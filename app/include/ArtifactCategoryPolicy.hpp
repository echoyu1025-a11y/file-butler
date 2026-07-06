// File Butler — customized edition. Maintained by qianyu.

#pragma once

#include <optional>
#include <string>

namespace ArtifactCategoryPolicy {

/**
 * @brief Normalized category/subcategory labels for software-like or archive-like files.
 */
struct NormalizedCategoryLabels {
    /**
     * @brief Stable main category selected for the artifact family.
     */
    std::string category;
    /**
     * @brief Specific subcategory retained from the model output when meaningful.
     */
    std::string subcategory;
};

/**
 * @brief Returns whether software/archive artifact normalization rules apply.
 * @param file_name File name or path fragment to inspect.
 * @return True when the file belongs to a supported software-like or archive-like family.
 */
bool is_supported_artifact_file_name(const std::string& file_name);

/**
 * @brief Applies structural normalization for software-like and archive-like labels.
 * @param file_name File name or path fragment to inspect.
 * @param category Parsed model category before normalization.
 * @param subcategory Parsed model subcategory before normalization.
 * @return Canonicalized labels when the artifact family is supported and the main category is
 *         a broad artifact-family label; `std::nullopt` otherwise.
 */
std::optional<NormalizedCategoryLabels> normalize_category_labels(const std::string& file_name,
                                                                  const std::string& category,
                                                                  const std::string& subcategory);

} // namespace ArtifactCategoryPolicy
