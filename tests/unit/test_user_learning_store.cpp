#include <catch2/catch_test_macros.hpp>

#include "CacheMaintenanceService.hpp"
#include "TestHelpers.hpp"
#include "TextEmbeddingService.hpp"
#include "UserLearningStore.hpp"

#include <filesystem>
#include <fstream>

TEST_CASE("UserLearningStore records approved mappings in a separate database")
{
    TempDir config_dir;
    UserLearningStore store(config_dir.path().string());
    REQUIRE(store.is_open());

    UserLearningStore::ApprovedMapping mapping;
    mapping.file_name = "ab_test_plan.pdf";
    mapping.file_type = FileType::File;
    mapping.dir_path = "/example";
    mapping.category = "Business ☁️";
    mapping.subcategory = "AB Testing ☁️";
    mapping.suggested_name = "ab_testing_plan.pdf";
    mapping.used_consistency_hints = true;

    std::string error;
    REQUIRE(store.record_approved_mapping(mapping, &error));
    CHECK(store.approved_example_count() == 1);

    const auto entry = store.find_taxonomy_entry(mapping.category, mapping.subcategory);
    REQUIRE(entry.has_value());
    CHECK(entry->category == mapping.category);
    CHECK(entry->subcategory == mapping.subcategory);
    CHECK(entry->example_count == 1);

    const auto examples = store.approved_examples();
    REQUIRE(examples.size() == 1);
    CHECK(examples.front().file_name == mapping.file_name);
    CHECK(examples.front().category == mapping.category);
    CHECK(examples.front().subcategory == mapping.subcategory);
    CHECK(examples.front().suggested_name == mapping.suggested_name);
    CHECK(examples.front().used_consistency_hints);

    UserLearningStore reloaded(config_dir.path().string());
    REQUIRE(reloaded.is_open());
    CHECK(reloaded.approved_example_count() == 1);
    REQUIRE(reloaded.find_taxonomy_entry(mapping.category, mapping.subcategory).has_value());
}

TEST_CASE("UserLearningStore updates repeated approvals without duplicating examples")
{
    TempDir config_dir;
    UserLearningStore store(config_dir.path().string());
    REQUIRE(store.is_open());

    UserLearningStore::ApprovedMapping mapping;
    mapping.file_name = "statement.pdf";
    mapping.file_type = FileType::File;
    mapping.dir_path = "/finance";
    mapping.category = "Finance";
    mapping.subcategory = "Statements";

    std::string error;
    REQUIRE(store.record_approved_mapping(mapping, &error));

    mapping.category = "Banking";
    mapping.subcategory = "Bank Statements";
    REQUIRE(store.record_approved_mapping(mapping, &error));

    CHECK(store.approved_example_count() == 1);

    const auto old_entry = store.find_taxonomy_entry("Finance", "Statements");
    REQUIRE(old_entry.has_value());
    CHECK(old_entry->example_count == 0);

    const auto new_entry = store.find_taxonomy_entry("Banking", "Bank Statements");
    REQUIRE(new_entry.has_value());
    CHECK(new_entry->example_count == 1);

    const auto examples = store.approved_examples();
    REQUIRE(examples.size() == 1);
    CHECK(examples.front().category == "Banking");
    CHECK(examples.front().subcategory == "Bank Statements");
}

TEST_CASE("UserLearningStore imports whitelist taxonomy candidates without duplicating entries")
{
    TempDir config_dir;
    UserLearningStore store(config_dir.path().string());
    REQUIRE(store.is_open());

    const std::vector<UserLearningStore::TaxonomyCandidate> candidates = {
        {"Manuals", "", "whitelist:Default"},
        {"Spreadsheets", "", "whitelist:Default"},
        {"Manuals", "", "whitelist:Documents"},
        {"Business ☁️", "", "whitelist:Cloud"}
    };

    std::string error;
    REQUIRE(store.import_taxonomy_candidates(candidates, &error));
    REQUIRE(store.import_taxonomy_candidates(candidates, &error));

    CHECK(store.taxonomy_entry_count() == 3);
    CHECK(store.approved_example_count() == 0);

    const auto manuals = store.find_taxonomy_entry("Manuals", "");
    REQUIRE(manuals.has_value());
    CHECK(manuals->source == "whitelist:Default");
    CHECK(manuals->example_count == 0);

    REQUIRE(store.find_taxonomy_entry("Business ☁️", "").has_value());
}

TEST_CASE("UserLearningStore stores embeddings for imported taxonomy candidates")
{
    TempDir config_dir;
    UserLearningStore store(config_dir.path().string());
    REQUIRE(store.is_open());

    std::string error;
    REQUIRE(store.import_taxonomy_candidates({{"Manuals", "Camera Guides", "whitelist:Default"}}, &error));

    const auto entry = store.find_taxonomy_entry("Manuals", "Camera Guides");
    REQUIRE(entry.has_value());
    const auto embedding = store.taxonomy_embedding(entry->id);
    REQUIRE(embedding.has_value());
    CHECK(embedding->taxonomy_entry_id == entry->id);
    CHECK(embedding->embedding_model == std::string(TextEmbeddingService::model_id()));
    CHECK(embedding->dimension == static_cast<int>(TextEmbeddingService::dimension()));
    CHECK(embedding->vector.size() == TextEmbeddingService::dimension());
    CHECK_FALSE(embedding->source_text_hash.empty());
    CHECK(store.taxonomy_embedding_count() == 1);
}

TEST_CASE("UserLearningStore preserves review-confirmed taxonomy source during whitelist import")
{
    TempDir config_dir;
    UserLearningStore store(config_dir.path().string());
    REQUIRE(store.is_open());

    std::string error;
    REQUIRE(store.import_taxonomy_candidates({{"Manuals", "", "whitelist:Default"}}, &error));

    UserLearningStore::ApprovedMapping mapping;
    mapping.file_name = "camera_manual.pdf";
    mapping.file_type = FileType::File;
    mapping.dir_path = "/docs";
    mapping.category = "Manuals";
    mapping.subcategory = "";
    REQUIRE(store.record_approved_mapping(mapping, &error));

    REQUIRE(store.import_taxonomy_candidates({{"Manuals", "", "whitelist:Default"}}, &error));

    const auto manuals = store.find_taxonomy_entry("Manuals", "");
    REQUIRE(manuals.has_value());
    CHECK(manuals->source == "review_confirmed");
    CHECK(manuals->example_count == 1);
    CHECK(store.taxonomy_entry_count() == 1);
    CHECK(store.approved_example_count() == 1);
}

TEST_CASE("UserLearningStore removes imported whitelist taxonomy candidates without touching approved examples")
{
    TempDir config_dir;
    UserLearningStore store(config_dir.path().string());
    REQUIRE(store.is_open());

    std::string error;
    REQUIRE(store.import_taxonomy_candidates({{"Manuals", "", "whitelist:Default"},
                                              {"Spreadsheets", "Budgets", "whitelist:Documents"},
                                              {"Configs", "", "user_taxonomy"}}, &error));

    UserLearningStore::ApprovedMapping mapping;
    mapping.file_name = "camera_manual.pdf";
    mapping.file_type = FileType::File;
    mapping.dir_path = "/docs";
    mapping.category = "Guides";
    mapping.subcategory = "Camera Manuals";
    REQUIRE(store.record_approved_mapping(mapping, &error));

    REQUIRE(store.remove_taxonomy_candidates_with_source_prefix("whitelist:", &error));

    CHECK_FALSE(store.find_taxonomy_entry("Manuals", "").has_value());
    CHECK_FALSE(store.find_taxonomy_entry("Spreadsheets", "Budgets").has_value());

    const auto kept_import = store.find_taxonomy_entry("Configs", "");
    REQUIRE(kept_import.has_value());
    CHECK(kept_import->source == "user_taxonomy");

    const auto kept_approved = store.find_taxonomy_entry("Guides", "Camera Manuals");
    REQUIRE(kept_approved.has_value());
    CHECK(kept_approved->source == "review_confirmed");
    CHECK(kept_approved->example_count == 1);
    CHECK(store.approved_example_count() == 1);
}

TEST_CASE("UserLearningStore refreshes embeddings when approved examples change")
{
    TempDir config_dir;
    UserLearningStore store(config_dir.path().string());
    REQUIRE(store.is_open());

    UserLearningStore::ApprovedMapping mapping;
    mapping.file_name = "camera_manual.pdf";
    mapping.file_type = FileType::File;
    mapping.dir_path = "/docs";
    mapping.category = "Manuals";
    mapping.subcategory = "Camera Guides";
    mapping.context_text = "Camera aperture and shutter instructions.";

    std::string error;
    REQUIRE(store.record_approved_mapping(mapping, &error));

    const auto entry = store.find_taxonomy_entry(mapping.category, mapping.subcategory);
    REQUIRE(entry.has_value());
    const auto before = store.taxonomy_embedding(entry->id);
    REQUIRE(before.has_value());

    mapping.context_text = "Lens cleaning and firmware update instructions.";
    REQUIRE(store.record_approved_mapping(mapping, &error));

    const auto after = store.taxonomy_embedding(entry->id);
    REQUIRE(after.has_value());
    CHECK(after->source_text_hash != before->source_text_hash);
    CHECK(after->vector.size() == before->vector.size());
}

TEST_CASE("UserLearningStore retrieves relevant taxonomy candidates from learned examples")
{
    TempDir config_dir;
    UserLearningStore store(config_dir.path().string());
    REQUIRE(store.is_open());

    std::string error;
    UserLearningStore::ApprovedMapping manual_mapping;
    manual_mapping.file_name = "nikon_camera_manual.pdf";
    manual_mapping.file_type = FileType::File;
    manual_mapping.dir_path = "/docs/cameras";
    manual_mapping.category = "Manuals";
    manual_mapping.subcategory = "Camera Guides";
    manual_mapping.context_text = "A product manual for a Nikon camera.";
    REQUIRE(store.record_approved_mapping(manual_mapping, &error));

    REQUIRE(store.import_taxonomy_candidates({{"Spreadsheets", "", "whitelist:Default"}}, &error));

    const auto candidates = store.retrieve_taxonomy_candidates("camera setup manual pdf", 3);
    REQUIRE_FALSE(candidates.empty());
    CHECK(candidates.front().category == "Manuals");
    CHECK(candidates.front().subcategory == "Camera Guides");
    CHECK(candidates.front().score > 0);
    CHECK(candidates.front().example_count == 1);
}

TEST_CASE("UserLearningStore uses stored embeddings during candidate retrieval")
{
    TempDir config_dir;
    UserLearningStore store(config_dir.path().string());
    REQUIRE(store.is_open());

    std::string error;
    UserLearningStore::ApprovedMapping manual_mapping;
    manual_mapping.file_name = "dslr_reference.pdf";
    manual_mapping.file_type = FileType::File;
    manual_mapping.dir_path = "/photo";
    manual_mapping.category = "Manuals";
    manual_mapping.subcategory = "Camera Guides";
    manual_mapping.context_text = "A DSLR reference about aperture shutter speed and lens setup.";
    REQUIRE(store.record_approved_mapping(manual_mapping, &error));

    REQUIRE(store.import_taxonomy_candidates({{"Spreadsheets", "Budgets", "whitelist:Default"}}, &error));

    const auto candidates = store.retrieve_taxonomy_candidates("aperture shutter lens setup", 3);
    REQUIRE_FALSE(candidates.empty());
    CHECK(candidates.front().category == "Manuals");
    CHECK(candidates.front().used_embedding);
    CHECK(candidates.front().embedding_similarity > 0.0);
}

TEST_CASE("UserLearningStore clears learned behavior while keeping the database reusable")
{
    TempDir config_dir;
    UserLearningStore store(config_dir.path().string());
    REQUIRE(store.is_open());

    std::string error;
    UserLearningStore::ApprovedMapping mapping;
    mapping.file_name = "manual.pdf";
    mapping.file_type = FileType::File;
    mapping.dir_path = "/docs";
    mapping.category = "Manuals";
    mapping.subcategory = "Product Guides";
    mapping.context_text = "Camera setup and maintenance guide.";
    REQUIRE(store.record_approved_mapping(mapping, &error));
    REQUIRE(store.import_taxonomy_candidates({{"Spreadsheets", "Budgets", "whitelist:Default"}}, &error));
    REQUIRE(store.taxonomy_entry_count() == 2);
    REQUIRE(store.approved_example_count() == 1);
    REQUIRE(store.taxonomy_embedding_count() == 2);

    REQUIRE(store.clear_all(&error));

    CHECK(std::filesystem::exists(store.database_path()));
    CHECK(store.taxonomy_entry_count() == 0);
    CHECK(store.approved_example_count() == 0);
    CHECK(store.taxonomy_embedding_count() == 0);

    REQUIRE(store.import_taxonomy_candidates({{"Manuals", "", "whitelist:Default"}}, &error));
    CHECK(store.taxonomy_entry_count() == 1);
}

TEST_CASE("CacheMaintenanceService does not remove learned user behavior with categorization cache")
{
    TempDir config_dir;
    const auto categorization_cache = config_dir.path() / "categorization_results.db";
    std::ofstream(categorization_cache).put('x');

    UserLearningStore store(config_dir.path().string());
    REQUIRE(store.is_open());
    UserLearningStore::ApprovedMapping mapping;
    mapping.file_name = "manual.pdf";
    mapping.file_type = FileType::File;
    mapping.dir_path = "/docs";
    mapping.category = "Manuals";
    mapping.subcategory = "Product Guides";
    std::string error;
    REQUIRE(store.record_approved_mapping(mapping, &error));
    REQUIRE(std::filesystem::exists(store.database_path()));

    CacheMaintenanceService service(config_dir.path().string());
    REQUIRE(service.clear(CacheMaintenanceTarget::Categorization, &error));

    CHECK_FALSE(std::filesystem::exists(categorization_cache));
    CHECK(std::filesystem::exists(store.database_path()));
    CHECK(store.approved_example_count() == 1);
}
