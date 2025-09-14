//
// Created by jvigu on 9/13/2025.
//

#ifndef TRIODE_TRIT_H
#define TRIODE_TRIT_H

#include <compare>
#include <cstdint>
#include <format>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <triode/arch/utils.h>


namespace triode::arch {
    enum class Trit : char {
        NEGATIVE = -1,
        ZERO = 0,
        POSITIVE = 1,
    };

    constexpr std::strong_ordering operator<=>(const Trit& lhs, const Trit& rhs) {
        return static_cast<char>(lhs) <=> static_cast<char>(rhs);
    }

    inline std::ostream& operator<<(std::ostream& os, Trit trit) {
        os << static_cast<int>(trit);
        return os;
    }

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

    /**
     * @class TritwiseOperations
     * @brief A mixin class providing trit manipulation and logical operations for Word-like structures.
     *
     * This version is specifically adapted to work with classes that have a public member
     * 'std::array<Tryte, N_TRYTES> trytes_'.
     *
     * @tparam Derived The class that is inheriting (e.g., Word).
     * @tparam N_TRYTES The number of trytes in the derived class.
     * @tparam N_TRITS_IN_TRYTE The number of trits per tryte.
     */
    template<typename Derived, size_t N_TRYTES, size_t N_TRITS_IN_TRYTE>
    class TritwiseOperations {
    private:
        static constexpr size_t N = N_TRYTES * N_TRITS_IN_TRYTE; // Total trits

    public:
        //=========================================================================
        // TRITWISE LOGICAL OPERATORS
        //=========================================================================

        Derived operator~() const {
            Derived result;
            const auto& self = static_cast<const Derived&>(*this);
            for (size_t i = 0; i < N_TRYTES; ++i) {
                for(size_t j = 0; j < N_TRITS_IN_TRYTE; ++j) {
                    switch (self.trytes_[i].trits_[j]) {
                        case Trit::POSITIVE: result.trytes_[i].trits_[j] = Trit::NEGATIVE; break;
                        case Trit::ZERO:     result.trytes_[i].trits_[j] = Trit::ZERO;     break;
                        case Trit::NEGATIVE: result.trytes_[i].trits_[j] = Trit::POSITIVE; break;
                    }
                }
            }
            return result;
        }

        Derived operator&(const Derived& other) const {
            Derived result;
            const auto& self = static_cast<const Derived&>(*this);
            for (size_t i = 0; i < N_TRYTES; ++i) {
                for(size_t j = 0; j < N_TRITS_IN_TRYTE; ++j) {
                    result.trytes_[i].trits_[j] = std::min(self.trytes_[i].trits_[j], other.trytes_[i].trits_[j]);
                }
            }
            return result;
        }

        Derived operator|(const Derived& other) const {
            Derived result;
            const auto& self = static_cast<const Derived&>(*this);
            for (size_t i = 0; i < N_TRYTES; ++i) {
                 for(size_t j = 0; j < N_TRITS_IN_TRYTE; ++j) {
                    result.trytes_[i].trits_[j] = std::max(self.trytes_[i].trits_[j], other.trytes_[i].trits_[j]);
                }
            }
            return result;
        }

    protected:
        //=========================================================================
        // HELPER FUNCTIONS for Instruction Packing/Unpacking
        //=========================================================================

        /**
         * @brief Extracts a range of trits from the nested tryte structure.
         */
        [[nodiscard]] int64_t get_trit_range(const size_t start_trit, const size_t num_trits) const {
            if (start_trit + num_trits > N) {
                throw std::out_of_range("Trit range is out of bounds.");
            }

            int64_t value = 0;
            int64_t power_of_3 = 1;
            const auto& self = static_cast<const Derived&>(*this);

            for (size_t i = 0; i < num_trits; ++i) {
                const size_t current_trit_index = start_trit + i;
                size_t tryte_index = current_trit_index / N_TRITS_IN_TRYTE;
                size_t trit_in_tryte_index = current_trit_index % N_TRITS_IN_TRYTE;

                value += static_cast<int8_t>(self.trytes_[tryte_index].trits_[trit_in_tryte_index]) * power_of_3;
                power_of_3 *= 3;
            }
            return value;
        }

        /**
         * @brief Sets a range of trits in the nested tryte structure.
         * This now uses the provided decimal_to_balanced_trits for conversion.
         */
        void set_trit_range(const size_t start_trit, const size_t num_trits, const int64_t value) {
            if (start_trit + num_trits > N) {
                throw std::out_of_range("Trit range is out of bounds.");
            }

            // We must use a switch statement here because the template parameter N
            // for decimal_to_balanced_trits must be a compile-time constant.
            std::vector<Trit> trits_to_set;
            switch(num_trits) {
                case 3:  { auto arr = decimal_to_balanced_trits<3>(value);  trits_to_set.assign(arr.begin(), arr.end()); break; }
                case 6:  { auto arr = decimal_to_balanced_trits<6>(value);  trits_to_set.assign(arr.begin(), arr.end()); break; }
                case 12: { auto arr = decimal_to_balanced_trits<12>(value); trits_to_set.assign(arr.begin(), arr.end()); break; }
                case 18: { auto arr = decimal_to_balanced_trits<18>(value); trits_to_set.assign(arr.begin(), arr.end()); break; }
                case 24: { auto arr = decimal_to_balanced_trits<24>(value); trits_to_set.assign(arr.begin(), arr.end()); break; }
                default:
                    throw std::invalid_argument("set_trit_range called with an unsupported number of trits.");
            }

            auto& self = static_cast<Derived&>(*this);

            for (size_t i = 0; i < num_trits; ++i) {
                const size_t current_trit_index = start_trit + i;
                size_t tryte_index = current_trit_index / N_TRITS_IN_TRYTE;
                size_t trit_in_tryte_index = current_trit_index % N_TRITS_IN_TRYTE;
                self.trytes_[tryte_index].trits_[trit_in_tryte_index] = trits_to_set[i];
            }
        }
    };
}

#endif //TRIODE_TRIT_H