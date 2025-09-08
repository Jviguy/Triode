//
// Created by jvigu on 9/7/2025.
//
#include <catch2/catch_all.hpp>
#include <triode/types.h>

using namespace triode::arch;

TEST_CASE("Word default constructor initializes to zero", "[word][constructor]") {
    const Word word;
    REQUIRE(word.to_int() == 0);
}

TEST_CASE("Word constructor correctly handles a positive value round-trip", "[word][constructor]") {
    constexpr int64_t test_value = 531441; // A nice round ternary number (3^12)
    const auto word = Word(test_value);
    REQUIRE(word.to_int() == test_value);
}

TEST_CASE("Word constructor correctly handles a negative value round-trip", "[word][constructor]") {
    constexpr int64_t test_value = -265720;
    const auto word = Word(test_value);
    REQUIRE(word.to_int() == test_value);
}

TEST_CASE("Word constructor handles the maximum value boundary", "[word][constructor]") {
    constexpr int64_t max_value = WORD_MAX;
    const auto word = Word(max_value);
    REQUIRE(word.to_int() == max_value);
}

TEST_CASE("Word constructor handles the minimum value boundary", "[word][constructor]") {
    constexpr int64_t min_value = WORD_MIN;
    const auto word = Word(min_value);
    REQUIRE(word.to_int() == min_value);
}

TEST_CASE("Word constructor throws exception for value greater than MAX", "[word][constructor][exceptions]") {
    // The largest valid value is WORD_MAX. We expect WORD_MAX + 1 to throw.
    REQUIRE_THROWS_AS(Word(WORD_MAX + 1), std::invalid_argument);
}

TEST_CASE("Word constructor throws exception for value less than MIN", "[word][constructor][exceptions]") {
    // The smallest valid value is WORD_MIN. We expect WORD_MIN - 1 to throw.
    REQUIRE_THROWS_AS(Word(WORD_MIN - 1), std::invalid_argument);
}