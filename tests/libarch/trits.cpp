//
// Created by jvigu on 9/7/2025.
//
#include <catch2/catch_all.hpp>
#include <triode/arch/trit.h>
#include  <triode/arch/tryte.h>
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

// A mock class to test the TritwiseOperations mixin, mimicking the Word structure.
constexpr size_t MOCK_WORD_TRYTES = 4;
constexpr size_t MOCK_WORD_TRITS = TRITS_IN_TRYTE * MOCK_WORD_TRYTES;

struct MockWord : TritwiseOperations<MockWord, MOCK_WORD_TRYTES, TRITS_IN_TRYTE> {
    std::array<Tryte, MOCK_WORD_TRYTES> trytes_{};

    // Default constructor to create a zeroed word
    MockWord() = default;

    // Construct from a 64-bit integer for easy test setup
    explicit MockWord(const int64_t val) {
        const auto trits = decimal_to_balanced_trits<MOCK_WORD_TRITS>(val);
        for(size_t i = 0; i < MOCK_WORD_TRITS; ++i) {
            const size_t tryte_idx = i / TRITS_IN_TRYTE;
            const size_t trit_idx = i % TRITS_IN_TRYTE;
            trytes_[tryte_idx].trits_[trit_idx] = trits[i];
        }
    }

    // Equality operator for comparisons in tests
    bool operator==(const MockWord& other) const {
        for(size_t i=0; i < MOCK_WORD_TRYTES; ++i) {
            if (trytes_[i] != other.trytes_[i]) return false;
        }
        return true;
    }

    //--- Testing Seam ---
    // Expose the protected methods for direct testing.
public:
    using TritwiseOperations::get_trit_range;
    using TritwiseOperations::set_trit_range;
};

TEST_CASE("TritwiseOperations: get_trit_range", "[trit_ops][get]") {
    // Value: 1 + (3 * 0) + (9 * -1) + (27*1) + ...
    // Trits (LST): {1, 0, -1, 1, ...}
    // Tryte 0: {1, -1, 0, 1, 0, 0} = 25
    // Tryte 1: {-1, -1, 1, 0, 0, 0} = 5
    // Tryte 2: {0, 1, 0, 0, 0, 0} = 3
    // Tryte 3: {0, 0, 0, 0, 0, 0} = 0
    MockWord test_word;
    test_word.trytes_[0] = Tryte(25);
    test_word.trytes_[1] = Tryte(5);
    test_word.trytes_[2] = Tryte(3);

    SECTION("Extracts a range within a single tryte") {
        // rd() field: trits 6-8. In Tryte 1, trits 0-2 = {-1, -1, 1} = 5
        REQUIRE(test_word.get_trit_range(6, 3) == 5);
    }

    SECTION("Extracts a range across tryte boundaries") {
        // trits 4-9
        // Tryte 0: trits 4, 5 = {0, 0}
        // Tryte 1: trits 0, 1, 2, 3 = {-1, -1, 1, 0}
        // Value = (0*1 + 0*3) + (-1*9) + (-1*27) + (1*81) + (0*243) = -36 + 81 = 45
        REQUIRE(test_word.get_trit_range(4, 6) == 45);
    }

    SECTION("Extracts the full value of a tryte") {
        REQUIRE(test_word.get_trit_range(0, 6) == 25);
        REQUIRE(test_word.get_trit_range(6, 6) == 5);
        REQUIRE(test_word.get_trit_range(12, 6) == 3);
    }

    SECTION("Throws when range is out of bounds") {
        REQUIRE_THROWS_AS(test_word.get_trit_range(20, 5), std::out_of_range);
    }
}

TEST_CASE("TritwiseOperations: set_trit_range", "[trit_ops][set]") {
    MockWord test_word;

    SECTION("Sets a value within a single tryte") {
        // Set trits 6-8 (rd field) to 7
        test_word.set_trit_range(6, 3, 7); // 7 = {1, -1, 1}
        REQUIRE(test_word.get_trit_range(6, 3) == 7);
        // Verify other trits are still zero
        REQUIRE(test_word.get_trit_range(0, 6) == 0);
        REQUIRE(test_word.get_trit_range(9, 3) == 0);
    }

    SECTION("Sets a value across tryte boundaries") {
        // Set trits 4-9 to 63
        test_word.set_trit_range(4, 6, 63);
        REQUIRE(test_word.get_trit_range(4, 6) == 63);
    }

    SECTION("Sets a negative value") {
        test_word.set_trit_range(0, 6, -25);
        REQUIRE(test_word.get_trit_range(0, 6) == -25);
    }

    SECTION("Throws when range is out of bounds") {
        REQUIRE_THROWS_AS(test_word.set_trit_range(20, 5, 10), std::out_of_range);
    }

    SECTION("Throws for unsupported number of trits") {
        REQUIRE_THROWS_AS(test_word.set_trit_range(0, 7, 10), std::invalid_argument);
    }
}

TEST_CASE("TritwiseOperations: Logical Operators", "[trit_ops][logical]") {
    // a = 13 = {1, 1, 1, 0, ...}
    // b = 5  = {-1, -1, 1, 0, ...}
    const MockWord a(13);
    const MockWord b(5);

    SECTION("Tritwise NOT (negation)") {
        MockWord not_a = ~a;
        MockWord expected_not_a(-13);
        REQUIRE(not_a == expected_not_a);
    }

    SECTION("Tritwise AND (min)") {
        MockWord a_and_b = a & b;
        // a: {1, 1, 1, 0...}
        // b: {-1,-1,1, 0...}
        // min: {-1,-1,1, 0...} => 5
        MockWord expected_and(5);
        REQUIRE(a_and_b == expected_and);
    }

    SECTION("Tritwise OR (max)") {
        MockWord a_or_b = a | b;
        // a: {1, 1, 1, 0...}
        // b: {-1,-1,1, 0...}
        // max: {1, 1, 1, 0...} => 13
        MockWord expected_or(13);
        REQUIRE(a_or_b == expected_or);
    }
}