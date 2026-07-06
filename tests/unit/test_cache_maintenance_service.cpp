#include <catch2/catch_test_macros.hpp>

#include "CacheMaintenanceService.hpp"
#include "TestHelpers.hpp"

#include <filesystem>
#include <fstream>
#include <sqlite3.h>
#include <string>

namespace {

void write_bytes(const std::filesystem::path& path, std::size_t count)
{
    std::filesystem::create_directories(path.parent_path());
    std::ofstream out(path, std::ios::binary);
    for (std::size_t index = 0; index < count; ++index) {
        out.put(static_cast<char>('a' + (index % 26)));
    }
}

void create_empty_categorization_cache(const std::filesystem::path& path)
{
    std::filesystem::create_directories(path.parent_path());
    sqlite3* db = nullptr;
    REQUIRE(sqlite3_open(path.string().c_str(), &db) == SQLITE_OK);
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS file_categorization (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            file_name TEXT NOT NULL,
            file_type TEXT NOT NULL,
            dir_path TEXT NOT NULL,
            category TEXT NOT NULL,
            subcategory TEXT,
            suggested_name TEXT,
            taxonomy_id INTEGER,
            categorization_style INTEGER DEFAULT 0,
            rename_only INTEGER DEFAULT 0,
            rename_applied INTEGER DEFAULT 0,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            UNIQUE(file_name, file_type, dir_path)
        );
    )";
    char* error = nullptr;
    REQUIRE(sqlite3_exec(db, sql, nullptr, nullptr, &error) == SQLITE_OK);
    if (error) {
        sqlite3_free(error);
    }
    REQUIRE(sqlite3_exec(db, "VACUUM;", nullptr, nullptr, &error) == SQLITE_OK);
    if (error) {
        sqlite3_free(error);
    }
    sqlite3_close(db);
}

} // namespace

TEST_CASE("CacheMaintenanceService reports cache paths and sizes")
{
    TempDir config_dir;
    TempDir log_dir;
    EnvVarGuard cache_file_guard("CATEGORIZATION_CACHE_FILE", std::string("custom-cache.db"));

    const auto categorization_path = config_dir.path() / "custom-cache.db";
    const auto image_location_path = config_dir.path() / "image_place_cache.db";
    const auto active_log_path = log_dir.path() / "core.log";
    const auto rotated_log_path = log_dir.path() / "core.log.1";

    write_bytes(categorization_path, 17);
    write_bytes(image_location_path, 9);
    write_bytes(active_log_path, 5);
    write_bytes(rotated_log_path, 11);

    CacheMaintenanceService service(
        config_dir.path().string(),
        {},
        [log_path = log_dir.path()]() {
            return log_path;
        });

    const auto categorization_info = service.target_info(CacheMaintenanceTarget::Categorization);
    REQUIRE(categorization_info.exists);
    CHECK(categorization_info.path == categorization_path);
    CHECK(categorization_info.size_bytes == 17);

    const auto image_location_info = service.target_info(CacheMaintenanceTarget::ImageLocation);
    REQUIRE(image_location_info.exists);
    CHECK(image_location_info.path == image_location_path);
    CHECK(image_location_info.size_bytes == 9);

    const auto logs_info = service.target_info(CacheMaintenanceTarget::Logs);
    REQUIRE(logs_info.exists);
    CHECK(logs_info.path == log_dir.path());
    CHECK(logs_info.size_bytes == 16);
}

TEST_CASE("CacheMaintenanceService clears configured cache targets")
{
    TempDir config_dir;
    TempDir log_dir;

    const auto categorization_path = config_dir.path() / "categorization_results.db";
    const auto image_location_path = config_dir.path() / "image_place_cache.db";
    const auto active_log_path = log_dir.path() / "core.log";
    const auto rotated_log_path = log_dir.path() / "core.log.1";
    const auto nested_log_dir = log_dir.path() / "archive";
    const auto nested_log_path = nested_log_dir / "old.log";

    write_bytes(categorization_path, 8);
    write_bytes(image_location_path, 6);
    write_bytes(active_log_path, 12);
    write_bytes(rotated_log_path, 7);
    write_bytes(nested_log_path, 4);

    CacheMaintenanceService service(
        config_dir.path().string(),
        {},
        [log_path = log_dir.path()]() {
            return log_path;
        });

    std::string error;
    REQUIRE(service.clear(CacheMaintenanceTarget::Categorization, &error));
    CHECK_FALSE(std::filesystem::exists(categorization_path));

    error.clear();
    REQUIRE(service.clear(CacheMaintenanceTarget::ImageLocation, &error));
    CHECK_FALSE(std::filesystem::exists(image_location_path));

    error.clear();
    REQUIRE(service.clear(CacheMaintenanceTarget::Logs, &error));
    REQUIRE(std::filesystem::exists(log_dir.path()));
    REQUIRE(std::filesystem::exists(active_log_path));
    CHECK(std::filesystem::file_size(active_log_path) == 0);
    CHECK_FALSE(std::filesystem::exists(rotated_log_path));
    CHECK_FALSE(std::filesystem::exists(nested_log_dir));
}

TEST_CASE("CacheMaintenanceService uses specialized clear callbacks when provided")
{
    TempDir config_dir;

    bool categorization_cleared = false;
    bool logs_cleared = false;
    CacheMaintenanceService service(
        config_dir.path().string(),
        CacheMaintenanceService::Callbacks{
            .clear_categorization_cache = [&](std::string&) {
                categorization_cleared = true;
                return true;
            },
            .clear_logs = [&](std::string&) {
                logs_cleared = true;
                return true;
            }});

    std::string error;
    REQUIRE(service.clear(CacheMaintenanceTarget::Categorization, &error));
    REQUIRE(categorization_cleared);

    error.clear();
    REQUIRE(service.clear(CacheMaintenanceTarget::Logs, &error));
    REQUIRE(logs_cleared);
}

TEST_CASE("CacheMaintenanceService reports zero size for an empty categorization database")
{
    TempDir config_dir;

    const auto categorization_path = config_dir.path() / "categorization_results.db";
    create_empty_categorization_cache(categorization_path);
    REQUIRE(std::filesystem::exists(categorization_path));
    REQUIRE(std::filesystem::file_size(categorization_path) > 0);

    CacheMaintenanceService service(config_dir.path().string());
    const auto categorization_info = service.target_info(CacheMaintenanceTarget::Categorization);

    REQUIRE(categorization_info.exists);
    CHECK(categorization_info.path == categorization_path);
    CHECK(categorization_info.size_bytes == 0);
}
