#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <future>

#include "GgufFileValidation.hpp"
#include "LLMDownloader.hpp"
#include "TestHelpers.hpp"
#include "TestHooks.hpp"
#include "Utils.hpp"

namespace {

void write_bytes(const std::filesystem::path& path, std::size_t count) {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    for (std::size_t i = 0; i < count; ++i) {
        out.put('x');
    }
}

void write_gguf_bytes(const std::filesystem::path& path, std::size_t count)
{
    std::filesystem::create_directories(path.parent_path());
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    out.write("GGUF", 4);
    for (std::size_t i = 4; i < count; ++i) {
        out.put('\0');
    }
}

void write_metadata(const std::filesystem::path& path,
                    const std::string& url,
                    long long content_length) {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream out(path, std::ios::trunc);
    out << "url=" << url << "\n";
    out << "content_length=" << content_length << "\n";
}

std::string make_unique_url() {
    return std::string("http://example.com/") + make_unique_token("model-") + ".gguf";
}

} // namespace

TEST_CASE("LLMDownloader retries full download after a range error") {
    TempDir tmp;
    const auto destination = (tmp.path() / "model.bin").string();
    const auto partial_destination = destination + ".part";

    {
        std::ofstream out(destination, std::ios::binary);
        out << "abc";
    }

    LLMDownloader downloader("http://example.com/model.bin");
    LLMDownloader::LLMDownloaderTestAccess::set_download_destination(downloader, destination);
    LLMDownloader::LLMDownloaderTestAccess::set_resume_headers(downloader, 6);

    std::atomic<int> attempts{0};
    TestHooks::set_llm_download_probe(
        [&](long offset, const std::string& path) -> CURLcode {
            ++attempts;
            if (attempts == 1) {
                REQUIRE(offset == 3);
                REQUIRE(path == partial_destination);
                return CURLE_HTTP_RANGE_ERROR;
            }
            REQUIRE(offset == 0);
            REQUIRE(path == partial_destination);
            std::ofstream out(path, std::ios::binary | std::ios::trunc);
            out << "abcdef";
            return CURLE_OK;
        });

    std::promise<void> done;
    std::future<void> finished = done.get_future();
    std::atomic<bool> success{false};
    std::string error_text;

    downloader.start_download(
        [](double) {},
        [&]() {
            success = true;
            done.set_value();
        },
        nullptr,
        [&](const std::string& err) {
            error_text = err;
            done.set_value();
        });

    const auto status = finished.wait_for(std::chrono::seconds(2));
    TestHooks::reset_llm_download_probe();
    REQUIRE(status == std::future_status::ready);
    REQUIRE(error_text.empty());
    REQUIRE(success.load());
    REQUIRE(attempts.load() == 2);
    REQUIRE(std::filesystem::file_size(destination) == 6);
    REQUIRE_FALSE(std::filesystem::exists(partial_destination));
}

TEST_CASE("LLMDownloader uses cached metadata for partial downloads") {
    TempDir tmp;
    EnvVarGuard home_guard("HOME", tmp.path().string());
#ifdef _WIN32
    EnvVarGuard appdata_guard("APPDATA", tmp.path().string());
    EnvVarGuard localappdata_guard("LOCALAPPDATA", tmp.path().string());
#endif

    const std::string url = make_unique_url();
    const std::filesystem::path destination =
        Utils::make_default_path_to_file_from_download_url(url);
    const std::filesystem::path metadata = destination.string() + ".aifs.meta";

    write_bytes(destination, 4);
    write_metadata(metadata, url, 16);

    LLMDownloader downloader(url, destination.string());
    CHECK(downloader.get_real_content_length() == 16);
    CHECK_FALSE(std::filesystem::exists(destination));
    CHECK(std::filesystem::exists(destination.string() + ".part"));
    CHECK(downloader.get_local_download_status() == LLMDownloader::DownloadStatus::InProgress);
    CHECK(downloader.get_download_status() == LLMDownloader::DownloadStatus::InProgress);
    CHECK_FALSE(downloader.is_inited());
}

TEST_CASE("LLMDownloader resets to not started when local file is missing") {
    TempDir tmp;
    EnvVarGuard home_guard("HOME", tmp.path().string());
#ifdef _WIN32
    EnvVarGuard appdata_guard("APPDATA", tmp.path().string());
    EnvVarGuard localappdata_guard("LOCALAPPDATA", tmp.path().string());
#endif

    const std::string url = make_unique_url();
    const std::filesystem::path destination =
        Utils::make_default_path_to_file_from_download_url(url);
    const std::filesystem::path metadata = destination.string() + ".aifs.meta";

    write_metadata(metadata, url, 16);

    LLMDownloader downloader(url, destination.string());
    CHECK(downloader.get_real_content_length() == 16);
    CHECK(downloader.get_local_download_status() == LLMDownloader::DownloadStatus::NotStarted);
    CHECK(downloader.get_download_status() == LLMDownloader::DownloadStatus::NotStarted);
    CHECK_FALSE(downloader.is_inited());
}

TEST_CASE("LLMDownloader treats full local file as complete with cached metadata") {
    TempDir tmp;
    EnvVarGuard home_guard("HOME", tmp.path().string());
#ifdef _WIN32
    EnvVarGuard appdata_guard("APPDATA", tmp.path().string());
    EnvVarGuard localappdata_guard("LOCALAPPDATA", tmp.path().string());
#endif

    const std::string url = make_unique_url();
    const std::filesystem::path destination =
        Utils::make_default_path_to_file_from_download_url(url);
    const std::filesystem::path metadata = destination.string() + ".aifs.meta";

    write_gguf_bytes(destination, 16);
    write_metadata(metadata, url, 16);

    LLMDownloader downloader(url, destination.string());
    CHECK(downloader.get_download_destination() == destination.string());
    REQUIRE(has_gguf_header(destination));
    CHECK(downloader.get_real_content_length() == 16);
    CHECK(downloader.get_local_download_status() == LLMDownloader::DownloadStatus::Complete);
    CHECK(downloader.get_download_status() == LLMDownloader::DownloadStatus::Complete);
    CHECK_FALSE(downloader.is_inited());
}

TEST_CASE("LLMDownloader keeps final model intact while partial temp download exists") {
    TempDir tmp;
    EnvVarGuard home_guard("HOME", tmp.path().string());
#ifdef _WIN32
    EnvVarGuard appdata_guard("APPDATA", tmp.path().string());
    EnvVarGuard localappdata_guard("LOCALAPPDATA", tmp.path().string());
#endif

    const std::string url = make_unique_url();
    const std::filesystem::path destination =
        Utils::make_default_path_to_file_from_download_url(url);
    const std::filesystem::path partial = destination.string() + ".part";
    const std::filesystem::path metadata = destination.string() + ".aifs.meta";

    write_bytes(destination, 16);
    write_bytes(partial, 4);
    write_metadata(metadata, url, 16);

    LLMDownloader downloader(url, destination.string());
    CHECK(downloader.get_local_download_status() == LLMDownloader::DownloadStatus::InProgress);
    CHECK_FALSE(downloader.is_download_complete());
    CHECK(std::filesystem::file_size(destination) == 16);
    CHECK(std::filesystem::file_size(partial) == 4);
}

TEST_CASE("LLMDownloader rejects short completed downloads before replacing the final file") {
    TempDir tmp;
    const auto destination = (tmp.path() / "model.gguf").string();
    const auto partial_destination = destination + ".part";

    write_gguf_bytes(destination, 16);

    LLMDownloader downloader(make_unique_url());
    LLMDownloader::LLMDownloaderTestAccess::set_download_destination(downloader, destination);
    LLMDownloader::LLMDownloaderTestAccess::set_resume_headers(downloader, 16);

    TestHooks::set_llm_download_probe(
        [&](long offset, const std::string& path) -> CURLcode {
            REQUIRE(offset == 0);
            REQUIRE(path == partial_destination);
            write_bytes(path, 1);
            return CURLE_OK;
        });

    std::promise<void> done;
    std::future<void> finished = done.get_future();
    std::string error_text;

    downloader.start_download(
        [](double) {},
        [&]() {
            done.set_value();
        },
        nullptr,
        [&](const std::string& err) {
            error_text = err;
            done.set_value();
        });

    const auto status = finished.wait_for(std::chrono::seconds(2));
    TestHooks::reset_llm_download_probe();
    REQUIRE(status == std::future_status::ready);
    CHECK_THAT(error_text,
               Catch::Matchers::ContainsSubstring("size mismatch"));
    CHECK(std::filesystem::file_size(destination) == 16);
    CHECK(std::filesystem::exists(partial_destination));
    CHECK(std::filesystem::file_size(partial_destination) == 1);
}

TEST_CASE("LLMDownloader rejects invalid GGUF downloads before replacing the final file") {
    TempDir tmp;
    const auto destination = (tmp.path() / "model.gguf").string();
    const auto partial_destination = destination + ".part";

    write_gguf_bytes(destination, 16);

    LLMDownloader downloader(make_unique_url());
    LLMDownloader::LLMDownloaderTestAccess::set_download_destination(downloader, destination);
    LLMDownloader::LLMDownloaderTestAccess::set_resume_headers(downloader, 16);

    TestHooks::set_llm_download_probe(
        [&](long offset, const std::string& path) -> CURLcode {
            REQUIRE(offset == 0);
            REQUIRE(path == partial_destination);
            write_bytes(path, 16);
            REQUIRE_FALSE(has_gguf_header(std::filesystem::path(path)));
            return CURLE_OK;
        });

    std::promise<void> done;
    std::future<void> finished = done.get_future();
    std::string error_text;

    downloader.start_download(
        [](double) {},
        [&]() {
            done.set_value();
        },
        nullptr,
        [&](const std::string& err) {
            error_text = err;
            done.set_value();
        });

    const auto status = finished.wait_for(std::chrono::seconds(2));
    TestHooks::reset_llm_download_probe();
    REQUIRE(status == std::future_status::ready);
    CHECK_THAT(error_text,
               Catch::Matchers::ContainsSubstring("expected GGUF header"));
    CHECK(std::filesystem::file_size(destination) == 16);
    CHECK(std::filesystem::exists(partial_destination));
    CHECK(std::filesystem::file_size(partial_destination) == 16);
}

TEST_CASE("LLMDownloader does not let callers retarget an active download") {
    TempDir tmp;
    LLMDownloader downloader(make_unique_url());
    LLMDownloader::LLMDownloaderTestAccess::set_download_destination(
        downloader, (tmp.path() / "model.gguf").string());

    std::promise<void> started;
    std::shared_future<void> started_future(started.get_future());
    std::promise<void> release;
    std::shared_future<void> release_future(release.get_future());
    std::promise<void> done;
    std::future<void> finished = done.get_future();

    TestHooks::set_llm_download_probe(
        [&](long, const std::string&) -> CURLcode {
            started.set_value();
            release_future.wait();
            return CURLE_ABORTED_BY_CALLBACK;
        });

    downloader.start_download(
        [](double) {},
        [&]() {
            done.set_value();
        },
        nullptr,
        [&](const std::string&) {
            done.set_value();
        });

    REQUIRE(started_future.wait_for(std::chrono::seconds(2)) == std::future_status::ready);
    CHECK_THROWS_WITH(
        downloader.set_download_url(make_unique_url()),
        Catch::Matchers::ContainsSubstring("download URL while a download is active"));

    downloader.cancel_download();
    release.set_value();
    REQUIRE(finished.wait_for(std::chrono::seconds(2)) == std::future_status::ready);
    TestHooks::reset_llm_download_probe();
}
