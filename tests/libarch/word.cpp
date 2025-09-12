//
// Created by jvigu on 9/7/2025.
//
#include <catch2/catch_all.hpp>
#include <triode/word.h>

using namespace triode::arch;

TEST_CASE("Word default constructor initializes to zero", "[word][constructor]") {
    constexpr Word word;
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

// Addition tests

TEST_CASE("Word full_add: Basic addition with zero carry-in", "[word][math][full_add]") {
    auto a = Word(1000);
    const auto b = Word(2000);

    // Perform the in-place addition: a = a + b + 0
    Trit carry_out = a.full_add(b, Trit::ZERO);

    REQUIRE(a.to_int() == 3000);
    REQUIRE(carry_out == Trit::ZERO); // No final carry expected
}

TEST_CASE("Word full_add: Addition with internal carry propagation", "[word][math][full_add]") {
    // 364 is TRYTE_MAX. Adding 1 will cause a carry from the first Tryte to the second.
    auto a = Word(364);
    const auto b = Word(1);

    Trit carry_out = a.full_add(b, Trit::ZERO);

    REQUIRE(a.to_int() == 365);
    REQUIRE(carry_out == Trit::ZERO); // No final carry expected
}

TEST_CASE("Word full_add: Overflow with zero carry-in", "[word][math][full_add]") {
    auto a = Word(WORD_MAX);
    const auto b = Word(1);

    Trit carry_out = a.full_add(b, Trit::ZERO);

    // The value of 'a' should wrap around to MIN
    REQUIRE(a.to_int() == WORD_MIN);
    // A positive carry should have been generated
    REQUIRE(carry_out == Trit::POSITIVE);
}

TEST_CASE("Word full_add: Basic addition with positive carry-in", "[word][math][full_add]") {
    auto a = Word(1000);
    const auto b = Word(2000);

    // Perform the in-place addition: a = a + b + 1
    Trit carry_out = a.full_add(b, Trit::POSITIVE);

    REQUIRE(a.to_int() == 3001);
    REQUIRE(carry_out == Trit::ZERO);
}

TEST_CASE("Word full_add: Overflow caused by positive carry-in", "[word][math][full_add]") {
    auto a = Word(WORD_MAX);
    const auto b = Word(0);

    // Perform the in-place addition: a = MAX + 0 + 1
    Trit carry_out = a.full_add(b, Trit::POSITIVE);

    REQUIRE(a.to_int() == WORD_MIN);
    REQUIRE(carry_out == Trit::POSITIVE);
}

TEST_CASE("Word full_add: Subtraction via negative carry-in", "[word][math][full_add]") {
    auto a = Word(1000);
    const auto b = Word(2000);

    // Perform the in-place addition: a = a + b - 1
    Trit carry_out = a.full_add(b, Trit::NEGATIVE);

    REQUIRE(a.to_int() == 2999);
    REQUIRE(carry_out == Trit::ZERO);
}

TEST_CASE("Word full_add: Underflow caused by negative carry-in", "[word][math][full_add]") {
    auto a = Word(WORD_MIN);
    const auto b = Word(0);

    // Perform the in-place addition: a = MIN + 0 - 1
    Trit carry_out = a.full_add(b, Trit::NEGATIVE);

    REQUIRE(a.to_int() == WORD_MAX);
    REQUIRE(carry_out == Trit::NEGATIVE);
}