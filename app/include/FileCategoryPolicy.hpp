#pragma once

#include "Types.hpp"

#include <optional>
#include <string>
#include <vector>

namespace FileCategoryPolicy {

/**
 * @brief Prompt-time main-category selection guidance for a coarse file family.
 */
struct MainCategorySelection {
    /**
     * @brief Descriptive family label used for logging and tests.
     */
    std::string family_name;
    /**
     * @brief Ordered main-category candidates for the current file family.
     */
    std::vector<std::string> categories;
};

/**
 * @brief Determines a bounded list of main-category candidates for an item.
 * @param file_name Prompt-facing file name or path fragment to inspect.
 * @param file_type File or directory type being categorized.
 * @return Candidate main categories ordered from most likely to broadest fallback.
 */
MainCategorySelection determine_main_category_selection(const std::string& file_name,
                                                        FileType file_type);

/**
 * @brief Returns whether the file name belongs to the shared image family rules.
 * @param file_name Prompt-facing file name or path fragment to inspect.
 * @return True when the extension matches the supported image-like set.
 */
bool is_supported_image_file_name(const std::string& file_name);

/**
 * @brief Returns whether the file name belongs to the shared document family rules.
 * @param file_name Prompt-facing file name or path fragment to inspect.
 * @return True when the extension matches the supported document-like set, including legacy Office binaries.
 */
bool is_supported_document_file_name(const std::string& file_name);

/**
 * @brief Returns the stable main category for shared image/document family files.
 * @param file_name Prompt-facing file name or path fragment to inspect.
 * @return Preferred stable main category for supported image/document files, or `std::nullopt` otherwise.
 */
std::optional<std::string> preferred_main_category_for_file_name(const std::string& file_name);

} // namespace FileCategoryPolicy
