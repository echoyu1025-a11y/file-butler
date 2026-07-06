#include "CleanupScanService.hpp"

#include <QByteArray>
#include <QCryptographicHash>
#include <QFile>
#include <QString>

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <map>
#include <system_error>
#include <vector>

/*
 * IMPORTANT: Everything in this file is read-only. There is no call to
 * std::filesystem::remove / rename / QFile::remove / rmdir / unlink anywhere.
 * The scanner only iterates directories, reads sizes, and hashes contents.
 */

namespace {

/// Extensions (lower-cased, with leading dot) treated as junk/cache.
constexpr std::array<const char*, 6> kJunkExtensions{
    ".tmp", ".temp", ".log", ".cache", ".bak", ".old"};

/// Exact filenames (lower-cased) treated as junk/cache.
constexpr std::array<const char*, 4> kJunkFilenames{
    ".ds_store", "thumbs.db", "desktop.ini", ".localized"};

std::string to_lower(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

std::uintmax_t safe_file_size(const std::filesystem::path& path)
{
    std::error_code ec;
    const std::uintmax_t size = std::filesystem::file_size(path, ec);
    return ec ? 0 : size;
}

/**
 * @brief Computes the SHA-256 of a file's contents in a streaming fashion.
 * @param path File to hash.
 * @param ok Set to false when the file could not be read.
 * @return Hex-encoded digest, or an empty string on failure.
 */
QByteArray sha256_of_file(const std::filesystem::path& path, bool& ok)
{
    ok = false;
    QFile file(QString::fromStdString(path.string()));
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }

    QCryptographicHash hash(QCryptographicHash::Sha256);
    constexpr qint64 kChunk = 1 << 20; // 1 MiB
    while (!file.atEnd()) {
        const QByteArray chunk = file.read(kChunk);
        if (chunk.isEmpty() && !file.atEnd()) {
            file.close();
            return {};
        }
        hash.addData(chunk);
    }
    file.close();
    ok = true;
    return hash.result();
}

bool is_directory_empty(const std::filesystem::path& path)
{
    std::error_code ec;
    std::filesystem::directory_iterator it(path, ec);
    if (ec) {
        return false;
    }
    return it == std::filesystem::directory_iterator();
}

} // namespace

bool CleanupScanService::is_junk_filename(const std::string& filename)
{
    const std::string lower = to_lower(filename);

    for (const char* name : kJunkFilenames) {
        if (lower == name) {
            return true;
        }
    }

    const std::string::size_type dot = lower.find_last_of('.');
    if (dot != std::string::npos) {
        const std::string ext = lower.substr(dot);
        for (const char* junk_ext : kJunkExtensions) {
            if (ext == junk_ext) {
                return true;
            }
        }
    }
    return false;
}

std::string CleanupScanService::format_size(std::uintmax_t size_bytes)
{
    static constexpr const char* kUnits[] = {"B", "KB", "MB", "GB", "TB"};
    double value = static_cast<double>(size_bytes);
    std::size_t unit_index = 0;
    while (value >= 1024.0 && unit_index + 1 < std::size(kUnits)) {
        value /= 1024.0;
        ++unit_index;
    }

    char buffer[32];
    if (unit_index == 0) {
        std::snprintf(buffer, sizeof(buffer), "%.0f %s", value, kUnits[unit_index]);
    } else {
        std::snprintf(buffer, sizeof(buffer), "%.1f %s", value, kUnits[unit_index]);
    }
    return std::string(buffer);
}

std::size_t CleanupScanResult::total_items() const
{
    return junk_cache.items.size() + duplicates.items.size() +
           large_files.items.size() + empty_items.items.size();
}

std::uintmax_t CleanupScanResult::total_reclaimable() const
{
    return junk_cache.total_reclaimable + duplicates.total_reclaimable +
           large_files.total_reclaimable + empty_items.total_reclaimable;
}

CleanupScanResult CleanupScanService::scan(const std::filesystem::path& root)
{
    CleanupScanResult result;
    result.scanned_root = root;
    result.junk_cache.category = CleanupCategory::JunkCache;
    result.duplicates.category = CleanupCategory::Duplicate;
    result.large_files.category = CleanupCategory::LargeFile;
    result.empty_items.category = CleanupCategory::EmptyItem;

    std::error_code root_ec;
    if (root.empty() || !std::filesystem::is_directory(root, root_ec) || root_ec) {
        return result;
    }

    // Collect regular files grouped by size for duplicate detection, and
    // classify junk / large / zero-byte candidates in a single pass.
    std::map<std::uintmax_t, std::vector<std::filesystem::path>> files_by_size;

    std::error_code walk_ec;
    for (std::filesystem::recursive_directory_iterator
             it(root, std::filesystem::directory_options::skip_permission_denied, walk_ec),
             end;
         it != end;
         it.increment(walk_ec)) {
        if (walk_ec) {
            walk_ec.clear();
            continue;
        }

        const std::filesystem::path entry_path = it->path();
        std::error_code type_ec;

        // Empty directories.
        if (it->is_directory(type_ec) && !type_ec) {
            if (is_directory_empty(entry_path)) {
                CleanupItem item;
                item.path = entry_path;
                item.parent_dir = entry_path.parent_path();
                item.size_bytes = 0;
                item.is_directory = true;
                result.empty_items.items.push_back(std::move(item));
            }
            continue;
        }

        if (!it->is_regular_file(type_ec) || type_ec) {
            continue; // Skip symlinks, sockets, etc.
        }

        const std::uintmax_t size = safe_file_size(entry_path);
        const std::string filename = entry_path.filename().string();
        const std::filesystem::path parent = entry_path.parent_path();

        // Junk / cache files.
        if (is_junk_filename(filename)) {
            CleanupItem item;
            item.path = entry_path;
            item.parent_dir = parent;
            item.size_bytes = size;
            result.junk_cache.items.push_back(std::move(item));
        }

        // Zero-byte files.
        if (size == 0) {
            CleanupItem item;
            item.path = entry_path;
            item.parent_dir = parent;
            item.size_bytes = 0;
            result.empty_items.items.push_back(std::move(item));
        }

        // Large files.
        if (size > kLargeFileThresholdBytes) {
            CleanupItem item;
            item.path = entry_path;
            item.parent_dir = parent;
            item.size_bytes = size;
            result.large_files.items.push_back(std::move(item));
        }

        // Candidate for duplicate detection (only non-empty files).
        if (size > 0) {
            files_by_size[size].push_back(entry_path);
        }
    }

    // Duplicate detection: only hash files whose size collides with another.
    for (const auto& [size, paths] : files_by_size) {
        if (paths.size() < 2) {
            continue;
        }

        std::map<QByteArray, std::vector<std::filesystem::path>> by_hash;
        for (const std::filesystem::path& p : paths) {
            bool ok = false;
            const QByteArray digest = sha256_of_file(p, ok);
            if (!ok) {
                continue;
            }
            by_hash[digest].push_back(p);
        }

        for (auto& [digest, group] : by_hash) {
            (void)digest;
            if (group.size() < 2) {
                continue;
            }
            // Keep the first path as the retained original; the rest are
            // reported as redundant copies (reported only, never deleted).
            std::sort(group.begin(), group.end());
            for (std::size_t i = 1; i < group.size(); ++i) {
                CleanupItem item;
                item.path = group[i];
                item.parent_dir = group[i].parent_path();
                item.size_bytes = size;
                // 只存原件文件名，"duplicate of" 的措辞由对话框层翻译（i18n）
                item.note = group.front().filename().string();
                result.duplicates.items.push_back(std::move(item));
            }
        }
    }

    // Sort large files by descending size.
    std::sort(result.large_files.items.begin(), result.large_files.items.end(),
              [](const CleanupItem& a, const CleanupItem& b) {
                  return a.size_bytes > b.size_bytes;
              });

    // Aggregate reclaimable totals per group.
    auto sum_group = [](CleanupGroup& group) {
        std::uintmax_t total = 0;
        for (const CleanupItem& item : group.items) {
            total += item.size_bytes;
        }
        group.total_reclaimable = total;
    };
    sum_group(result.junk_cache);
    sum_group(result.duplicates);
    sum_group(result.large_files);
    sum_group(result.empty_items);

    return result;
}
