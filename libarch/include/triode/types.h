//
// Created by jvigu on 9/7/2025.
//

#ifndef TRIODE_TYPES_H
#define TRIODE_TYPES_H
#include <array>
#include <cstdint>
#include <format>
#include <iostream>
#include <stdexcept>
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

    constexpr int64_t pow3(const int exp) {
        int64_t result = 1;
        for (int i = 0; i < exp; ++i) {
            result *= 3;
        }
        return result;
    }

    // Tries to store a decimal value into a buffer of N trits.
    // Errors if the value can't fit into N trits.
    // Stores these values in LST (Least Significant Trit).
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

    constexpr int TRYTE_MAX = (pow3(TRITS_IN_TRYTE) - 1)/2;
    constexpr int TRYTE_MIN = -TRYTE_MAX;

    struct Tryte {
        std::array<Trit, TRITS_IN_TRYTE> trits{};

        Tryte() {
            trits.fill(Trit::ZERO);
        };

        Tryte(std::initializer_list<int>);
        explicit Tryte(const std::array<Trit, TRITS_IN_TRYTE> array) : trits(array) {};

        explicit Tryte(int);

        [[nodiscard]] int to_int() const;

        [[nodiscard]] Tryte negate() const {
            Tryte result;
            for (size_t i = 0; i < TRITS_IN_TRYTE; ++i) {
                if (trits[i] == Trit::POSITIVE) {
                    result.trits[i] = Trit::NEGATIVE;
                } else if (trits[i] == Trit::NEGATIVE) {
                    result.trits[i] = Trit::POSITIVE;
                } else {
                    result.trits[i] = Trit::ZERO;
                }
            }
            return result;
        }

        [[nodiscard]] std::pair<Tryte, Trit> full_add() const;

        auto operator<=>(const Tryte& rhs) const = default;

        auto operator+=(const Tryte& rhs) -> Tryte& {
            auto carry = Trit::ZERO;
            for (size_t i = 0; i < TRITS_IN_TRYTE; ++i) {
                int sum = static_cast<int>(this->trits[i]) +
                          static_cast<int>(rhs.trits[i]) +
                          static_cast<int>(carry);

                carry = Trit::ZERO;
                if (sum > 1) {
                    sum -= 3;
                    carry = Trit::POSITIVE;
                } else if (sum < -1) {
                    sum += 3;
                    carry = Trit::NEGATIVE;
                }
                this->trits[i] = static_cast<Trit>(sum);
            }
            return *this;
        }

        auto operator-=(const Tryte& rhs) -> Tryte& {
            *this += rhs.negate();
            return *this;
        }
    };

    [[nodiscard]] std::pair<Tryte, Trit> Tryte::full_add() const {

    }

    inline auto operator+(const Tryte& lhs, const Tryte& rhs) {
        Tryte result;
        auto carry = Trit::ZERO;

        for (size_t i = 0; i < TRITS_IN_TRYTE; ++i) {
            int sum = static_cast<int>(lhs.trits[i]) + static_cast<int>(rhs.trits[i]) + static_cast<int>(carry);
            carry = Trit::ZERO;
            if (sum > 1) {
                sum -= 3;
                carry = Trit::POSITIVE;
            } else if (sum < -1) {
                sum += 3;
                carry = Trit::NEGATIVE;
            }
            result.trits[i] = static_cast<Trit>(sum);
        }
        // we could check carry here for overflow saving, but I feel allowing overflow feels more like normal computing.
        return result;
    }

    inline auto operator-(const Tryte& tryte) {
        return tryte.negate();
    }


    inline std::ostream& operator<<(std::ostream& os, const Tryte& tryte) {
        os << "{ ";
        for (size_t i = 0; i < TRITS_IN_TRYTE; ++i) {
            os << static_cast<int>(tryte.trits[i]) << (i < tryte.trits.size() - 1 ? ", " : "");
        }
        os << " }";
        return os;
    }

    constexpr int TRYTES_IN_WORD = 4;
    constexpr int TRITS_IN_WORD = TRITS_IN_TRYTE * TRYTES_IN_WORD;
    constexpr int64_t WORD_MAX = (pow3(TRITS_IN_WORD) - 1) / 2;
    constexpr int64_t WORD_MIN = -WORD_MAX;

    class DoubleWord;

    class Word {
    public:
        std::array<Tryte, TRYTES_IN_WORD> trytes{};
        Word() = default;

        explicit Word(const std::array<Tryte, TRYTES_IN_WORD> &trytes) : trytes(trytes) {};
        explicit Word(int64_t n);

        [[nodiscard]] int64_t to_int() const;

        [[nodiscard]] Word add(const Word& other) const {
            Word res;
            auto carry = Tryte(0);
            for (size_t i = 0; i < TRYTES_IN_WORD; ++i) {
                auto sum = this->trytes[i] + other.trytes[i] + carry;
                carry = Tryte(0);
                if (sum.to_int() > TRYTE_MAX) {
                }
            }
            return res;
        }
    };

    class DoubleWord : public Word {
    public:
        std::array<Tryte, TRYTES_IN_WORD * 2> trytes{};
    };
}

#endif //TRIODE_TYPES_H