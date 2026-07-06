// File Butler — customized edition. Maintained by qianyu.

#pragma once

#include <filesystem>
#include <optional>
#include <string_view>
#include <vector>

namespace GgmlRuntimePaths {

bool has_payload(const std::filesystem::path& dir);

/**
 * @brief Returns candidate packaged CPU runtime directories for Windows.
 *
 * The returned list prefers the dedicated CPU runtime layout and then falls
 * back to the packaged Vulkan runtime layout so launcher-based builds can
 * reuse `wvulkan` when it already contains the CPU backend DLLs.
 *
 * @param exe_path Path to the currently running executable.
 * @return Candidate Windows CPU runtime directories in priority order.
 */
std::vector<std::filesystem::path> windows_cpu_runtime_candidate_dirs(
    const std::filesystem::path& exe_path);

/**
 * @brief Resolves the best packaged CPU runtime directory for Windows.
 *
 * @param exe_path Path to the currently running executable.
 * @return The first existing CPU runtime directory that contains the required
 * Windows CPU backend DLLs, or `std::nullopt` when none are usable.
 */
std::optional<std::filesystem::path> resolve_windows_cpu_runtime_dir(
    const std::filesystem::path& exe_path);

/**
 * @brief Returns candidate packaged Vulkan payload directories for Windows.
 *
 * The returned list prefers the BLAS-enabled Vulkan payload layout and falls
 * back to the legacy Vulkan directory layout for compatibility.
 *
 * @param exe_path Path to the currently running executable.
 * @return Candidate Windows Vulkan payload directories in priority order.
 */
std::vector<std::filesystem::path> windows_vulkan_payload_candidate_dirs(
    const std::filesystem::path& exe_path);

/**
 * @brief Resolves the best packaged Vulkan payload directory for Windows.
 *
 * @param exe_path Path to the currently running executable.
 * @return The first existing Vulkan payload directory that contains the
 * required Windows runtime DLLs, or `std::nullopt` when none are usable.
 */
std::optional<std::filesystem::path> resolve_windows_vulkan_payload_dir(
    const std::filesystem::path& exe_path);

std::vector<std::filesystem::path> macos_candidate_dirs(
    const std::filesystem::path& exe_path,
    std::string_view ggml_subdir);

std::optional<std::filesystem::path> resolve_macos_backend_dir(
    const std::optional<std::filesystem::path>& current_dir,
    const std::filesystem::path& exe_path,
    std::string_view ggml_subdir);

} // namespace GgmlRuntimePaths
