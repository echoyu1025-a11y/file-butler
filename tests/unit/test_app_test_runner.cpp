// File Butler — customized edition. Maintained by qianyu.

#include <catch2/catch_test_macros.hpp>

#include "AppTestRunner.hpp"

TEST_CASE("AppTestRunner runs whitelist self-test suite") {
    AppTestRunner runner;

    const auto result = runner.run(AppTestRunner::Options{.suite = "whitelist"});

    REQUIRE(result.error.empty());
    REQUIRE(result.cases.size() == 3);
    CHECK(result.passed());
}

TEST_CASE("AppTestRunner rejects unknown self-test suite") {
    AppTestRunner runner;

    const auto result = runner.run(AppTestRunner::Options{.suite = "missing-suite"});

    CHECK_FALSE(result.passed());
    CHECK(result.cases.empty());
    CHECK(result.error.find("Unknown test suite") != std::string::npos);
}
