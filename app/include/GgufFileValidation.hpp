/**
 * @file GgufFileValidation.hpp
 * @brief Lightweight helpers for validating GGUF model artifacts.
 */
#pragma once

#include <algorithm>
#include <array>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>

/**
 * @brief Return true when the path points to a `.gguf` artifact.
 * @param path File path to inspect.
 * @return True when the file extension is `.gguf`, case-insensitively.
 */
inline bool is_gguf_file_path(const std::filesystem::path& path)
{
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return extension == ".gguf";
}

/**
 * @brief Return true when the file begins with the GGUF magic header.
 * @param path File path to inspect.
 * @return True when the file can be opened and starts with `GGUF`.
 */
inline bool has_gguf_header(const std::filesystem::path& path)
{
    std::ifstream stream(path, std::ios::binary);
    if (!stream) {
        return false;
    }

    std::array<char, 4> magic{};
    stream.read(magic.data(), static_cast<std::streamsize>(magic.size()));
    return stream.gcount() == static_cast<std::streamsize>(magic.size())
        && std::string_view(magic.data(), magic.size()) == "GGUF";
}
