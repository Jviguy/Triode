#ifndef TRIODE_TYPES_H
#define TRIODE_TYPES_H

#include <array>
#include <cstdint>
#include <format>
#include <iostream>
#include <triode/utils.h>
// NOTE: All types here are generated in Little-Endian.

namespace triode::arch {
    enum class Trit : char {
        NEGATIVE = -1,
        ZERO = 0,
        POSITIVE = 1,
    };

    constexpr auto operator<=>(const Trit& lhs, const Trit& rhs) {
        return static_cast<char>(lhs) <=> static_cast<char>(rhs);
    }

    constexpr size_t TRITS_IN_TRYTE = 6;

    constexpr int TRYTE_MAX = (pow3(TRITS_IN_TRYTE) - 1)/2;
    constexpr int TRYTE_MIN = -TRYTE_MAX;

    struct Tryte {
        std::array<Trit, TRITS_IN_TRYTE> trits_{};

        Tryte() = default;

        Tryte(std::initializer_list<int>);

        // Constructs a tryte from an array of trits. The array must be exactly TRITS_IN_TRYTE long.
        Tryte(const std::array<Trit, TRITS_IN_TRYTE> array) : trits_{array} {}; // NOLINT
        // Constructs a tryte from a decimal value. See limits TRYTE_MAX and TRYTE_MIN.
        Tryte(int); //NOLINT

        Tryte(const Tryte& other) = default;
        Tryte(Tryte&& other) noexcept = default;
        Tryte& operator=(const Tryte& other) = default;
        Tryte& operator=(Tryte&& other) noexcept = default;
        ~Tryte() = default;

        [[nodiscard]] int to_int() const;

        [[nodiscard]] Tryte negate() const;

        // No lint here as Trit is the carry. In some cases like Tryte + Tryte, we don't need the carry.
        //NOLINTNEXTLINE
        Trit full_add(const Tryte& other, const Trit carry_in = Trit::ZERO);

        auto operator<=>(const Tryte& rhs) const = default;

        auto operator+=(const Tryte &rhs) -> Tryte&;

        auto operator-=(const Tryte& rhs) -> Tryte&;
    };


    auto operator+(const Tryte& lhs, const Tryte& rhs) -> Tryte;

    auto operator-(const Tryte& tryte) -> Tryte;

    std::ostream& operator<<(std::ostream& os, const Tryte& tryte);


    // Tries to store a decimal value into a buffer of N trits.
    // Errors if the value can't fit into N trits.
    // Stores these values in Little Endian
    template <size_t N>
    std::array<Trit, N> decimal_to_balanced_trits(int64_t n) {
        if (n == 0) {
            return std::array<Trit, N>{};
        }
        const bool is_negative = n < 0;
        n = std::abs(n);
        // this also checks minimum as n is absolute value and boundaries are symmetrical because balanced format.
        if (const int64_t max = (pow3(N) - 1) / 2; n > max) {
            throw std::invalid_argument(std::format("Cannot fit decimal provided into {} trits", N));
        }
        std::array<int, N> temp{};
        int ptr = 0;
        while (n > 0 && ptr < N) {
            temp[ptr++] = n % 3;
            n /= 3;
        }
        // 2a:
        std::array<Trit, N> result{};
        int carry = 0;
        for (int i = 0; i < N; i++) {
            int digit = temp[i] + carry;
            carry = 0;
            if (digit == 2) {
                result[i] = Trit::NEGATIVE;
                carry = 1;
            } else if (digit == 3) {
                result[i] = Trit::ZERO;
                carry = 1;
            } else {
                result[i] = static_cast<Trit>(digit);
            }
        }
        // final carry means overflow. We check this at the start but also worth it to check here.
        if (carry == 1) {
            throw std::invalid_argument(std::format("Cannot fit decimal provided into {} trits", N));
        }
        // 2b: do inversion to handle negative values.
        if (is_negative) {
            for (int i = 0; i < N; i++) {
                if (result[i] == Trit::NEGATIVE) {
                    result[i] = Trit::POSITIVE;
                } else if (result[i] == Trit::POSITIVE) {
                    result[i] = Trit::NEGATIVE;
                }
            }
        }
        return result;
    }
}
#endif //TRIODE_TYPES_H