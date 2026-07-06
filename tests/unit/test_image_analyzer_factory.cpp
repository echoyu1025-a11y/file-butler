#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "ImageAnalyzerFactory.hpp"
#include "TestHelpers.hpp"
#include "VisualModelCatalog.hpp"
#include "VisualLlmRuntime.hpp"

#include <fstream>

TEST_CASE("ImageAnalyzerFactory rejects invalid GGUF artifacts before analyzer startup")
{
    TempDir temp;

    const auto* descriptor = find_visual_model_descriptor("gemma-3-4b-it");
    REQUIRE(descriptor != nullptr);
    REQUIRE(descriptor->artifacts.size() == 2);

    const auto model_path = temp.path() / "model.gguf";
    const auto mmproj_path = temp.path() / "mmproj.gguf";
    std::ofstream(model_path, std::ios::binary).put('x');
    std::ofstream(mmproj_path, std::ios::binary).put('x');

    VisualLlmRuntime::Backend backend;
    backend.descriptor = descriptor;
    backend.artifacts.push_back({&descriptor->artifacts[0], model_path});
    backend.artifacts.push_back({&descriptor->artifacts[1], mmproj_path});

    ImageAnalyzerSettings settings;
    settings.use_gpu = false;

    REQUIRE_THROWS_WITH(
        ImageAnalyzerFactory::create(backend, settings),
        Catch::Matchers::ContainsSubstring(
            "Visual model artifact is invalid or incomplete (expected GGUF header):"));
}
