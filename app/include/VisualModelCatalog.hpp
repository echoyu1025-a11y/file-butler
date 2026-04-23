/**
 * @file VisualModelCatalog.hpp
 * @brief Descriptor catalog for supported local visual model backends.
 */
#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string_view>
#include <vector>

/**
 * @brief Supported local visual inference stacks.
 */
enum class VisualModelArchitecture {
    MtmdProjector,
};

/**
 * @brief Prompt policy used when talking to a visual backend.
 */
enum class VisualPromptPolicy {
    /** @brief Legacy prompt wording tuned for the LLaVA 1.6 family. */
    LegacyLlava,
    /** @brief General instruction-tuned multimodal prompt wording. */
    StructuredVisionInstruct,
};

/**
 * @brief Artifact kinds required by visual model backends.
 */
enum class VisualModelArtifactKind {
    Model,
    Mmproj,
};

/**
 * @brief Descriptor for a required visual model artifact.
 */
struct VisualModelArtifactDescriptor {
    /** @brief Artifact kind used by runtime/factory code. */
    VisualModelArtifactKind kind;
    /** @brief User-facing artifact name. */
    const char* display_name;
    /** @brief Environment variable containing the artifact download URL. */
    const char* url_env;
    /** @brief Stable backend-specific storage filename used for new downloads. */
    const char* local_storage_name;
    /** @brief Optional fallback filenames searched in the default model directory. */
    std::vector<std::string_view> fallback_filenames;
};

/**
 * @brief Optional runtime limits for a visual model backend.
 */
struct VisualModelRuntimeHints {
    /** @brief Maximum image tokens to request from dynamic-resolution projectors (0 = backend default). */
    int32_t image_max_tokens = 0;
    /** @brief Maximum visual evaluation batch size (0 = runtime default). */
    int32_t max_batch_size = 0;
};

/**
 * @brief Descriptor for a supported visual model backend.
 */
struct VisualModelDescriptor {
    /** @brief Stable backend identifier. */
    const char* id;
    /** @brief User-facing backend display name. */
    const char* display_name;
    /** @brief Architecture family used to instantiate the analyzer. */
    VisualModelArchitecture architecture;
    /** @brief Prompt policy used for image description / rename prompts. */
    VisualPromptPolicy prompt_policy;
    /** @brief Required artifacts for the backend. */
    std::vector<VisualModelArtifactDescriptor> artifacts;
    /** @brief Optional backend-specific runtime limits. */
    VisualModelRuntimeHints runtime_hints;
};

/**
 * @brief Return all built-in visual model descriptors.
 * @return Descriptor list in priority order.
 */
const std::vector<VisualModelDescriptor>& visual_model_descriptors();

/**
 * @brief Find a built-in visual model descriptor by id.
 * @param id Stable backend identifier to resolve.
 * @return Matching descriptor, or nullptr when not found.
 */
const VisualModelDescriptor* find_visual_model_descriptor(std::string_view id);

/**
 * @brief Return the currently preferred built-in visual model descriptor.
 * @return Default visual model descriptor.
 */
const VisualModelDescriptor& default_visual_model_descriptor();

/**
 * @brief Return the canonical on-disk location for a visual model artifact.
 * @param backend Backend descriptor owning the artifact.
 * @param artifact Artifact descriptor to resolve.
 * @return Backend-specific storage path for new downloads.
 */
std::filesystem::path visual_artifact_storage_path(const VisualModelDescriptor& backend,
                                                   const VisualModelArtifactDescriptor& artifact);

/**
 * @brief Resolve an existing visual artifact, including legacy flat-file installs.
 * @param backend Backend descriptor owning the artifact.
 * @param artifact Artifact descriptor to resolve.
 * @param download_url Download URL currently configured for the artifact.
 * @return Resolved on-disk artifact path when available.
 */
std::optional<std::filesystem::path> resolve_visual_artifact_path(
    const VisualModelDescriptor& backend,
    const VisualModelArtifactDescriptor& artifact,
    std::string_view download_url);
