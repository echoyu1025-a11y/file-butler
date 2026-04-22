#include <catch2/catch_test_macros.hpp>

#include "MainApp.hpp"
#include "MainAppTestAccess.hpp"
#include "Settings.hpp"
#include "TestHelpers.hpp"

#include <string>

TEST_CASE("Visual CPU fallback detection recognizes retryable GPU failures") {
    CHECK(MainAppTestAccess::should_offer_visual_cpu_fallback(
        "Failed to create llama_context (try AI_FILE_SORTER_VISUAL_USE_GPU=0 to force CPU)"));
    CHECK(MainAppTestAccess::should_offer_visual_cpu_fallback("mtmd_helper_eval_chunks failed"));
    CHECK(MainAppTestAccess::should_offer_visual_cpu_fallback("VK_ERROR_OUT_OF_DEVICE_MEMORY"));
    CHECK(MainAppTestAccess::should_offer_visual_cpu_fallback("CUDA error out of memory"));
}

TEST_CASE("Visual CPU fallback detection ignores non-retryable startup failures") {
    CHECK_FALSE(MainAppTestAccess::should_offer_visual_cpu_fallback(
        "Failed to load multimodal projector file at C:/models/mmproj.gguf"));
    CHECK_FALSE(MainAppTestAccess::should_offer_visual_cpu_fallback(
        "Failed to load visual text model at C:/models/model.gguf"));
    CHECK_FALSE(MainAppTestAccess::should_offer_visual_cpu_fallback(
        "The provided multimodal projector does not expose vision capabilities"));
}

#ifndef _WIN32
TEST_CASE("Visual CPU fallback decline requests analysis cancellation")
{
    EnvVarGuard platform_guard("QT_QPA_PLATFORM", "offscreen");
    QtAppContext qt_context;

    TempDir temp;
    EnvVarGuard home_guard("HOME", temp.path().string());
    EnvVarGuard config_guard("AI_FILE_SORTER_CONFIG_DIR", temp.path().string());

    Settings settings;
    REQUIRE(settings.save());
    MainApp window(settings, /*development_mode=*/false);

    int prompt_count = 0;
    MainAppTestAccess::set_visual_cpu_fallback_prompt_override(window, [&prompt_count]() {
        ++prompt_count;
        return false;
    });

    CHECK_FALSE(MainAppTestAccess::prompt_visual_cpu_fallback(window, "VK_ERROR_OUT_OF_DEVICE_MEMORY"));
    CHECK(MainAppTestAccess::stop_analysis_requested(window));
    CHECK(prompt_count == 1);

    CHECK_FALSE(MainAppTestAccess::prompt_visual_cpu_fallback(window, "later retry"));
    CHECK(prompt_count == 1);
}

TEST_CASE("Visual CPU fallback acceptance keeps analysis running")
{
    EnvVarGuard platform_guard("QT_QPA_PLATFORM", "offscreen");
    QtAppContext qt_context;

    TempDir temp;
    EnvVarGuard home_guard("HOME", temp.path().string());
    EnvVarGuard config_guard("AI_FILE_SORTER_CONFIG_DIR", temp.path().string());

    Settings settings;
    REQUIRE(settings.save());
    MainApp window(settings, /*development_mode=*/false);

    MainAppTestAccess::set_visual_cpu_fallback_prompt_override(window, []() { return true; });

    CHECK(MainAppTestAccess::prompt_visual_cpu_fallback(window, "VK_ERROR_OUT_OF_DEVICE_MEMORY"));
    CHECK_FALSE(MainAppTestAccess::stop_analysis_requested(window));
}
#endif
