#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

/**
 * @brief Deterministic local text-vector generator for learned categorization retrieval.
 *
 * This is intentionally lightweight and dependency-free. It provides a stable
 * embedding interface for persistence and ranking now, and can be replaced by a
 * neural embedding backend later without changing the learning database callers.
 */
class TextEmbeddingService {
public:
    /**
     * @brief Return the active local embedding model/version id.
     * @return Stable model id stored alongside persisted vectors.
     */
    static std::string_view model_id();
    /**
     * @brief Return the number of dimensions produced by this model.
     * @return Fixed vector dimension.
     */
    static std::size_t dimension();
    /**
     * @brief Generate a normalized deterministic text vector.
     * @param text Text to embed.
     * @return Normalized vector with `dimension()` values, or all zeros for empty text.
     */
    static std::vector<float> embed(std::string_view text);
    /**
     * @brief Compute cosine similarity between two embedding vectors.
     * @param lhs First vector.
     * @param rhs Second vector.
     * @return Similarity in the range [-1, 1], or 0 for invalid/empty vectors.
     */
    static double cosine_similarity(const std::vector<float>& lhs, const std::vector<float>& rhs);
    /**
     * @brief Compute a stable source-text hash for stale embedding detection.
     * @param text Source text used to build an embedding.
     * @return Fixed-width hexadecimal FNV-1a hash.
     */
    static std::string source_hash(std::string_view text);
};
