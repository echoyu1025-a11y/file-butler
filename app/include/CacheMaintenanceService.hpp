/**
 * @file CacheMaintenanceService.hpp
 * @brief Filesystem-backed cache inspection and cleanup helpers.
 */
#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>

/**
 * @brief Supported cache targets exposed by the Settings cleanup dialog.
 */
enum class CacheMaintenanceTarget {
    Categorization,
    ImageLocation,
    Logs
};

/**
 * @brief Filesystem metadata for a single cache target.
 */
struct CacheMaintenanceTargetInfo {
    CacheMaintenanceTarget target{CacheMaintenanceTarget::Categorization};
    std::filesystem::path path;
    std::uintmax_t size_bytes{0};
    bool exists{false};
};

/**
 * @brief Computes cache paths, estimates reclaimable size, and clears cache targets.
 */
class CacheMaintenanceService {
public:
    /**
     * @brief Optional specialized clear callbacks for targets that need app-aware cleanup.
     */
    struct Callbacks {
        std::function<bool(std::string&)> clear_categorization_cache;
        std::function<bool(std::string&)> clear_logs;
    };

    /**
     * @brief Constructs the cache maintenance helper.
     * @param config_dir Base configuration directory for cache databases.
     * @param callbacks Optional target-specific clear callbacks.
     * @param log_dir_provider Optional provider for the active log directory.
     */
    explicit CacheMaintenanceService(
        std::string config_dir,
        Callbacks callbacks = {},
        std::function<std::filesystem::path()> log_dir_provider = {});

    /**
     * @brief Returns the current path and size estimate for a cache target.
     * @param target Cache target to inspect.
     * @return Metadata describing the target path and reclaimable size.
     */
    CacheMaintenanceTargetInfo target_info(CacheMaintenanceTarget target) const;

    /**
     * @brief Clears the selected cache target.
     * @param target Cache target to clear.
     * @param error Optional output for a user-facing failure message.
     * @return True when the target was cleared successfully.
     */
    bool clear(CacheMaintenanceTarget target, std::string* error = nullptr) const;

    /**
     * @brief Formats a byte count as a compact human-readable size string.
     * @param size_bytes Size in bytes.
     * @return Formatted size string such as `12.4 MB`.
     */
    static std::string format_size(std::uintmax_t size_bytes);

private:
    /**
     * @brief Resolves the filesystem path backing a cache target.
     * @param target Cache target to resolve.
     * @return Backing filesystem path, or an empty path when unavailable.
     */
    std::filesystem::path target_path(CacheMaintenanceTarget target) const;
    /**
     * @brief Computes the size of a file or directory recursively.
     * @param path Path to inspect.
     * @return Total size in bytes, or `0` when the path is missing.
     */
    static std::uintmax_t compute_size_bytes(const std::filesystem::path& path);
    /**
     * @brief Estimates the logical size of an SQLite-backed cache target.
     * @param target Cache target whose backing database should be inspected.
     * @param path SQLite database path.
     * @return Zero when the cache table is empty; otherwise the on-disk file size estimate.
     */
    static std::uintmax_t compute_sqlite_cache_size(CacheMaintenanceTarget target,
                                                    const std::filesystem::path& path);
    /**
     * @brief Removes a single file-backed cache if it exists.
     * @param path File path to delete.
     * @param error Optional output for a user-facing failure message.
     * @return True when the file was removed or was already absent.
     */
    static bool remove_file_cache(const std::filesystem::path& path, std::string* error);
    /**
     * @brief Removes the contents of a directory-backed cache while keeping the directory.
     * @param path Directory path to clear.
     * @param error Optional output for a user-facing failure message.
     * @return True when the directory contents were removed or the directory was absent.
     */
    static bool clear_directory_cache(const std::filesystem::path& path, std::string* error);

    std::string config_dir_;
    Callbacks callbacks_;
    std::function<std::filesystem::path()> log_dir_provider_;
};
