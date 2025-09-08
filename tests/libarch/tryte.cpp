//
// Created by jvigu on 9/7/2025.
//
#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <triode/types.h>

using namespace triode::arch;

TEST_CASE("Tryte constructor (LST) handles zero correctly", "[tryte][constructor]") {
    auto tryte = Tryte(0);
    REQUIRE(tryte == Tryte{0, 0, 0, 0, 0, 0});
}

TEST_CASE("Tryte constructor (LST) handles a simple positive value", "[tryte][constructor]") {
    auto tryte = Tryte(25);
    // Trits, from least to most significant: {1, -1, 0, 1, 0, 0}
    REQUIRE(tryte == Tryte{1, -1, 0, 1, 0, 0});
}

TEST_CASE("Tryte constructor (LST) handles a simple negative value", "[tryte][constructor]") {
    auto tryte = Tryte(-25);
    // Trits, from least to most significant: {-1, 1, 0, -1, 0, 0}
    REQUIRE(tryte == Tryte{-1, 1, 0, -1, 0, 0});
}

TEST_CASE("Tryte constructor (LST) handles the maximum value boundary", "[tryte][constructor]") {
    auto max = Tryte(TRYTE_MAX); // 364
    REQUIRE(max == Tryte{1, 1, 1, 1, 1, 1});
}

TEST_CASE("Tryte constructor (LST) handles the minimum value boundary", "[tryte][constructor]") {
    auto min = Tryte(TRYTE_MIN); // -364
    REQUIRE(min == Tryte{-1, -1, -1, -1, -1, -1});
}

TEST_CASE("Tryte constructor (LST) handles carry propagation correctly", "[tryte][constructor]") {
    auto tryte = Tryte(2);
    // Trits, from least to most significant: {-1, 1, 0, 0, 0, 0}
    REQUIRE(tryte == Tryte{-1, 1, 0, 0, 0, 0});
}

TEST_CASE("Tryte constructor throws exception for value greater than MAX", "[tryte][constructor][exceptions]") {
    REQUIRE_THROWS_AS(Tryte(TRYTE_MAX + 1), std::invalid_argument);
}

TEST_CASE("Tryte constructor throws exception for value less than MIN", "[tryte][constructor][exceptions]") {
    REQUIRE_THROWS_AS(Tryte(TRYTE_MIN - 1), std::invalid_argument);
}

// --- Tests for addition.

TEST_CASE("Tryte Addition: Adding zero returns the original number", "[tryte][math][addition]") {
    // Tests the identity property of addition (A + 0 = A)
    const auto a = Tryte(123);
    const auto b = Tryte(0);
    auto result = a + b;
    REQUIRE(result == a);
}

TEST_CASE("Tryte Addition: Adding two positive numbers", "[tryte][math][addition]") {
    const auto a = Tryte(10);
    const auto b = Tryte(15);
    auto result = a + b;
    REQUIRE(result == Tryte(25));
}

TEST_CASE("Tryte Addition: Adding a positive and a negative number", "[tryte][math][addition]") {
    const auto a = Tryte(100);
    const auto b = Tryte(-25);
    auto result = a + b;
    REQUIRE(result == Tryte(75));
}

TEST_CASE("Tryte Addition: Adding two negative numbers", "[tryte][math][addition]") {
    const auto a = Tryte(-10);
    const auto b = Tryte(-15);
    auto result = a + b;
    REQUIRE(result == Tryte(-25));
}

TEST_CASE("Tryte Addition: Overflow wraps from MAX to MIN", "[tryte][math][addition]") {
    // Tests the wrap-around behavior for positive overflow
    const auto max = Tryte(TRYTE_MAX); // 364
    const auto one = Tryte(1);
    auto result = max + one;
    REQUIRE(result == Tryte(TRYTE_MIN)); // Should be -364
}

TEST_CASE("Tryte Addition: Underflow wraps from MIN to MAX", "[tryte][math][addition]") {
    // Tests the wrap-around behavior for negative overflow (underflow)
    const auto min = Tryte(TRYTE_MIN); // -364
    const auto neg_one = Tryte(-1);
    auto result = min + neg_one;
    REQUIRE(result == Tryte(TRYTE_MAX)); // Should be 364
}

// --- Negation operator testing

TEST_CASE("Tryte Negation: A positive number becomes negative", "[tryte][math][negation]") {
    const auto a = Tryte(25);
    const auto result = a.negate();
    REQUIRE(result == Tryte(-25));
}

TEST_CASE("Tryte Negation: A negative number becomes positive", "[tryte][math][negation]") {
    const auto a = Tryte(-25);
    const auto result = a.negate();
    REQUIRE(result == Tryte(25));
}

TEST_CASE("Tryte Negation: Zero remains zero", "[tryte][math][negation]") {
    // Tests the identity property for zero
    const auto a = Tryte(0);
    const auto result = a.negate();
    REQUIRE(result == Tryte(0));
}

TEST_CASE("Tryte Negation: Negating MAX results in MIN", "[tryte][math][negation]") {
    // A key property of a balanced system where MIN = -MAX
    const auto max = Tryte(TRYTE_MAX);
    const auto result = max.negate();
    REQUIRE(result == Tryte(TRYTE_MIN));
}

// --- Multiplication Testing
TEST_CASE("Tryte Multiplication: Multiplying by zero results in zero", "[tryte][math][multiplication]") {
    const auto a = Tryte(123);
    const auto b = Tryte(0);
    REQUIRE((a * b) == Tryte(0));
    REQUIRE((b * a) == Tryte(0));
}

TEST_CASE("Tryte Multiplication: Multiplying by one returns the original number", "[tryte][math][multiplication]") {
    // Tests the identity property for multiplication (A * 1 = A)
    const auto a = Tryte(42);
    const auto b = Tryte(1);
    REQUIRE((a * b) == a);
}

TEST_CASE("Tryte Multiplication: Multiplying by negative one returns the negated number", "[tryte][math][multiplication]") {
    // (A * -1 = -A)
    const auto a = Tryte(55);
    const auto b = Tryte(-1);
    REQUIRE((a * b) == Tryte(-55));
    REQUIRE((a * b) == a.negate());
}

TEST_CASE("Tryte Multiplication: Two positive numbers", "[tryte][math][multiplication]") {
    const auto a = Tryte(10);
    const auto b = Tryte(15);
    REQUIRE((a * b) == Tryte(150));
}

TEST_CASE("Tryte Multiplication: A positive and a negative number", "[tryte][math][multiplication]") {
    const auto a = Tryte(20);
    const auto b = Tryte(-5);
    REQUIRE((a * b) == Tryte(-100));
}

TEST_CASE("Tryte Multiplication: Two negative numbers", "[tryte][math][multiplication]") {
    const auto a = Tryte(-7);
    const auto b = Tryte(-8);
    REQUIRE((a * b) == Tryte(56));
}