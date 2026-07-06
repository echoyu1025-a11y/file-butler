/**
 * @file CleanupScanService.hpp
 * @brief Read-only recursive scanner that identifies cleanup candidates.
 *
 * IMPORTANT: This service is strictly read-only. It only stats files and
 * hashes their contents to detect duplicates. It never deletes, moves,
 * renames, or otherwise modifies any file or directory. Removal is left
 * entirely to the user via their system file manager.
 */
#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

/**
 * @brief The four categories of cleanup candidates the scanner reports.
 */
enum class CleanupCategory {
    JunkCache,      ///< Junk/cache files (temp, log, .DS_Store, Thumbs.db, ...).
    Duplicate,      ///< Byte-identical duplicate copies (originals excluded).
    LargeFile,      ///< Files larger than the large-file threshold (100 MB).
    EmptyItem       ///< Empty directories and zero-byte files.
};

/**
 * @brief A single cleanup candidate located during a scan.
 */
struct CleanupItem {
    std::filesystem::path path;         ///< Absolute path of the file or directory.
    std::filesystem::path parent_dir;   ///< Directory to reveal in the file manager.
    std::uintmax_t size_bytes{0};       ///< Reclaimable size in bytes.
    bool is_directory{false};           ///< True for empty directories.
    std::string note;                   ///< Optional short annotation (e.g. duplicate hint).
};

/**
 * @brief One category group with its items and aggregate statistics.
 */
struct CleanupGroup {
    CleanupCategory category{CleanupCategory::JunkCache};
    std::vector<CleanupItem> items;         ///< Candidates in this group.
    std::uintmax_t total_reclaimable{0};    ///< Sum of reclaimable bytes for the group.
};

/**
 * @brief Complete result of a cleanup scan across all four categories.
 */
struct CleanupScanResult {
    std::filesystem::path scanned_root;         ///< Root that was scanned.
    CleanupGroup junk_cache;                    ///< Junk/cache group.
    CleanupGroup duplicates;                    ///< Duplicate-copy group.
    CleanupGroup large_files;                   ///< Large-file group.
    CleanupGroup empty_items;                   ///< Empty directory / zero-byte group.

    /** @brief Total number of cleanup items across all groups. */
    std::size_t total_items() const;
    /** @brief Total reclaimable bytes across all groups. */
    std::uintmax_t total_reclaimable() const;
};

/**
 * @brief Recursively scans a folder for cleanup candidates. Read-only.
 */
class CleanupScanService {
public:
    /** @brief Files strictly larger than this many bytes count as "large". */
    static constexpr std::uintmax_t kLargeFileThresholdBytes = 100ull * 1024ull * 1024ull;

    /**
     * @brief Recursively scans @p root and classifies cleanup candidates.
     *
     * Performs only read operations (directory iteration, file_size, and
     * SHA-256 hashing of same-size files for duplicate detection). Never
     * modifies the filesystem.
     *
     * @param root Directory to scan.
     * @return Structured, grouped scan result.
     */
    static CleanupScanResult scan(const std::filesystem::path& root);

    /**
     * @brief Formats a byte count as a compact human-readable size string.
     * @param size_bytes Size in bytes.
     * @return Formatted string such as `1.2 GB`.
     */
    static std::string format_size(std::uintmax_t size_bytes);

    /**
     * @brief Returns true when a filename matches the junk/cache rules.
     * @param filename Base filename (no directory component).
     * @return True when the file is considered junk/cache.
     */
    static bool is_junk_filename(const std::string& filename);
};
