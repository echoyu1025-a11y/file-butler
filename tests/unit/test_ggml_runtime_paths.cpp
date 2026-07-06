// File Butler — customized edition. Maintained by qianyu.

#include <catch2/catch_test_macros.hpp>

#include "GgmlRuntimePaths.hpp"
#include "TestHelpers.hpp"

#include <algorithm>
#include <fstream>

TEST_CASE("macOS ggml runtime candidates stay relative to the app layout") {
    const std::filesystem::path exe =
        "/tmp/AIFileSorter.app/Contents/MacOS/aifilesorter";

    const auto candidates =
        GgmlRuntimePaths::macos_candidate_dirs(exe, "precompiled-m2");

    REQUIRE_FALSE(candidates.empty());
    REQUIRE(candidates[0] ==
            std::filesystem::path("/tmp/AIFileSorter.app/Contents/MacOS/../lib/precompiled-m1"));
    REQUIRE(candidates[1] ==
            std::filesystem::path("/tmp/AIFileSorter.app/Contents/MacOS/../lib/precompiled-m2"));
    REQUIRE(candidates[5] ==
            std::filesystem::path("/tmp/AIFileSorter.app/Contents/MacOS/../lib/aifilesorter"));
    REQUIRE(candidates[6] ==
            std::filesystem::path("/tmp/AIFileSorter.app/Contents/MacOS/../../lib/aifilesorter"));
    REQUIRE(std::find(candidates.begin(),
                      candidates.end(),
                      std::filesystem::path("/usr/local/lib")) == candidates.end());
    REQUIRE(std::find(candidates.begin(),
                      candidates.end(),
                      std::filesystem::path("/opt/homebrew/lib")) == candidates.end());
}

TEST_CASE("Windows Vulkan payload candidates prefer the BLAS runtime layout") {
    const std::filesystem::path exe = R"(C:\AIFileSorter\aifilesorter.exe)";

    const auto candidates = GgmlRuntimePaths::windows_vulkan_payload_candidate_dirs(exe);

    REQUIRE(candidates.size() == 2);
    REQUIRE(candidates[0] ==
            std::filesystem::path(R"(C:\AIFileSorter\lib\precompiled\vulkan-blas\bin)"));
    REQUIRE(candidates[1] ==
            std::filesystem::path(R"(C:\AIFileSorter\lib\precompiled\vulkan\bin)"));
}

TEST_CASE("Windows CPU runtime candidates fall back to the Vulkan runtime layout") {
    const std::filesystem::path exe = R"(C:\AIFileSorter\aifilesorter.exe)";

    const auto candidates = GgmlRuntimePaths::windows_cpu_runtime_candidate_dirs(exe);

    REQUIRE(candidates.size() == 4);
    REQUIRE(candidates[0] ==
            std::filesystem::path(R"(C:\AIFileSorter\lib\ggml\wocuda)"));
    REQUIRE(candidates[1] ==
            std::filesystem::path(R"(C:\AIFileSorter\ggml\wocuda)"));
    REQUIRE(candidates[2] ==
            std::filesystem::path(R"(C:\AIFileSorter\lib\ggml\wvulkan)"));
    REQUIRE(candidates[3] ==
            std::filesystem::path(R"(C:\AIFileSorter\ggml\wvulkan)"));
}

TEST_CASE("Windows CPU runtime resolution falls back to the Vulkan runtime layout") {
    TempDir temp_dir;
    const auto root = temp_dir.path();
    const auto exe = root / "aifilesorter.exe";
    const auto fallback = root / "lib" / "ggml" / "wvulkan";

    std::ofstream(exe).put('x');

    std::filesystem::create_directories(fallback);
    std::ofstream(fallback / "llama.dll").put('x');
    std::ofstream(fallback / "ggml.dll").put('x');
    std::ofstream(fallback / "ggml-cpu.dll").put('x');

    const auto resolved = GgmlRuntimePaths::resolve_windows_cpu_runtime_dir(exe);

    REQUIRE(resolved.has_value());
    REQUIRE(*resolved == fallback);
}

TEST_CASE("Windows Vulkan payload resolution prefers the BLAS runtime layout") {
    TempDir temp_dir;
    const auto root = temp_dir.path();
    const auto exe = root / "aifilesorter.exe";
    const auto preferred = root / "lib" / "precompiled" / "vulkan-blas" / "bin";
    const auto fallback = root / "lib" / "precompiled" / "vulkan" / "bin";

    std::ofstream(exe).put('x');

    std::filesystem::create_directories(preferred);
    std::filesystem::create_directories(fallback);
    for (const auto& dir : {preferred, fallback}) {
        std::ofstream(dir / "llama.dll").put('x');
        std::ofstream(dir / "ggml.dll").put('x');
        std::ofstream(dir / "ggml-vulkan.dll").put('x');
        std::ofstream(dir / "vulkan-1.dll").put('x');
    }

    const auto resolved = GgmlRuntimePaths::resolve_windows_vulkan_payload_dir(exe);

    REQUIRE(resolved.has_value());
    REQUIRE(*resolved == preferred);
}

TEST_CASE("macOS ggml runtime resolution prefers bundled directories over generic siblings") {
    TempDir temp_dir;
    const auto root = temp_dir.path();
    const auto exe = root / "bin" / "m2" / "aifilesorter";
    const auto bundled = root / "lib" / "precompiled-m2";
    const auto generic = root / "lib" / "aifilesorter";

    std::filesystem::create_directories(exe.parent_path());
    std::ofstream(exe).put('x');

    std::filesystem::create_directories(bundled);
    std::ofstream(bundled / "libggml-metal.dylib").put('x');

    std::filesystem::create_directories(generic);
    std::ofstream(generic / "libggml-cpu.dylib").put('x');

    const auto resolved = GgmlRuntimePaths::resolve_macos_backend_dir(
        std::nullopt,
        exe,
        "precompiled-m2");

    REQUIRE(resolved.has_value());
    REQUIRE(*resolved == bundled);
}

TEST_CASE("macOS ggml runtime resolution preserves a valid explicit override") {
    TempDir temp_dir;
    const auto root = temp_dir.path();
    const auto exe = root / "bin" / "aifilesorter";
    const auto custom = root / "custom-ggml";

    std::filesystem::create_directories(exe.parent_path());
    std::ofstream(exe).put('x');

    std::filesystem::create_directories(custom);
    std::ofstream(custom / "libggml-blas.dylib").put('x');

    const auto resolved = GgmlRuntimePaths::resolve_macos_backend_dir(
        custom,
        exe,
        "precompiled");

    REQUIRE(resolved.has_value());
    REQUIRE(*resolved == custom);
}
