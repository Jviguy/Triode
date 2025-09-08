//
// Created by jvigu on 9/7/2025.
//
#include <catch2/catch_all.hpp>
#include <triode/types.h>
using namespace triode::arch;


TEST_CASE("decimal_to_balanced_trits (LST) handles zero", "[helper][conversion]") {
    auto trits = Tryte{decimal_to_balanced_trits<TRITS_IN_TRYTE>(0)};
    REQUIRE(trits == Tryte{0, 0, 0, 0, 0, 0});
}

TEST_CASE("decimal_to_balanced_trits (LST) handles a simple positive value", "[helper][conversion]") {
    auto trits = Tryte{decimal_to_balanced_trits<TRITS_IN_TRYTE>(25)};
    // Expected: {1, -1, 0, 1, 0, 0} from least to most significant
    REQUIRE(trits == Tryte{1, -1, 0, 1, 0, 0});
}

TEST_CASE("decimal_to_balanced_trits (LST) handles a simple negative value", "[helper][conversion]") {
    auto trits = Tryte{decimal_to_balanced_trits<TRITS_IN_TRYTE>(-25)};
    // Expected: {-1, 1, 0, -1, 0, 0} from least to most significant
    REQUIRE(trits == Tryte{-1, 1, 0, -1, 0, 0});
}

TEST_CASE("decimal_to_balanced_trits (LST) handles carry propagation", "[helper][conversion]") {
    // 5 = (1 * 9) + (-1 * 3) + (-1 * 1) = 9 - 3 - 1
    auto trits = Tryte{decimal_to_balanced_trits<TRITS_IN_TRYTE>(5)};
    // Expected: {-1, -1, 1, 0, 0, 0} from least to most significant
    REQUIRE(trits == Tryte{-1, -1, 1, 0, 0, 0});
}

TEST_CASE("decimal_to_balanced_trits throws for value greater than MAX", "[helper][conversion][exceptions]") {
    // 364 is the max for 6 trits. We expect 365 to throw.
    REQUIRE_THROWS_AS(decimal_to_balanced_trits<TRITS_IN_TRYTE>(TRYTE_MAX + 1), std::invalid_argument);
}

TEST_CASE("decimal_to_balanced_trits throws for value less than MIN", "[helper][conversion][exceptions]") {
    // -364 is the min for 6 trits. We expect -365 to throw.
    REQUIRE_THROWS_AS(decimal_to_balanced_trits<TRITS_IN_TRYTE>(TRYTE_MIN - 1), std::invalid_argument);
}