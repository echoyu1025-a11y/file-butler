#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace WindowsCudaProbe {

struct ProbeResult {
    bool driver_present{false};
    bool driver_initialized{false};
    int driver_version{0};
    int device_count{0};
    bool runtime_present{false};
    bool runtime_usable{false};
    bool backend_loadable{false};
    int runtime_version_token{0};
    std::filesystem::path runtime_library_path;
    std::string failure_reason;
};

ProbeResult probe(const std::optional<std::filesystem::path>& ggml_directory = std::nullopt);
std::optional<std::filesystem::path> best_runtime_library_path();
int installed_runtime_version_token();
std::string best_runtime_library_name();

#ifdef AI_FILE_SORTER_TEST_BUILD
namespace TestAccess {
int runtime_version_rank(std::string_view file_name);
std::vector<std::filesystem::path> rank_runtime_candidates(
    const std::vector<std::filesystem::path>& runtime_paths);
} // namespace TestAccess
#endif

} // namespace WindowsCudaProbe
