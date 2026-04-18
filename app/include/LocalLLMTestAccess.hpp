#pragma once

#ifdef AI_FILE_SORTER_TEST_BUILD

#include "LocalLLMClient.hpp"
#include "Types.hpp"

#include <optional>
#include <string>
#include "llama.h"

namespace LocalLLMTestAccess {

enum class BackendPreference {
    Auto,
    Cpu,
    Cuda,
    Vulkan
};

BackendPreference detect_preferred_backend();
bool apply_cpu_backend(llama_model_params& params, BackendPreference preference);
bool apply_vulkan_backend(const std::string& model_path,
                          llama_model_params& params);
bool handle_cuda_forced_off(bool cuda_forced_off,
                            BackendPreference preference,
                            llama_model_params& params);
bool configure_cuda_backend(const std::string& model_path,
                            llama_model_params& params);
/**
 * @brief Prepared model parameters plus any pre-load status emitted during backend selection.
 */
struct PreparedModelParamsResult {
    llama_model_params params;
    std::optional<LocalLLMClient::Status> status;
};
/**
 * @brief Builds model parameters and captures preflight backend status for tests.
 * @param model_path Path to the GGUF model used for backend preparation.
 * @return Prepared parameters together with any emitted status.
 */
PreparedModelParamsResult prepare_model_params_result_for_testing(const std::string& model_path);
llama_model_params prepare_model_params_for_testing(const std::string& model_path);
std::string categorization_system_prompt_for_testing(const std::string& file_path,
                                                     FileType file_type);
std::string categorization_user_prompt_for_testing(const std::string& file_name,
                                                   const std::string& file_path,
                                                   FileType file_type,
                                                   const std::string& consistency_context);
std::string sanitize_output_for_testing(const std::string& output);

} // namespace LocalLLMTestAccess

#endif // AI_FILE_SORTER_TEST_BUILD
