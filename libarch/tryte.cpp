//
// Created by jvigu on 9/7/2025.
//
#include <triode/tryte.h>

namespace triode::arch {

    Tryte::Tryte(const std::initializer_list<int> list) {
        trits_.fill(Trit::ZERO); // Start with a zeroed array
        size_t i = 0;
        // Copy elements from the list, but don't go past the end of our array
        for (int val : list) {
            if (i >= TRITS_IN_TRYTE) break;
            trits_[i++] = static_cast<Trit>(val);
        }
    }

    Tryte::Tryte(const int n) {
        if (n < TRYTE_MIN || n > TRYTE_MAX) {
            throw std::invalid_argument("Invalid decimal value to put into one tryte.");
        }
        trits_ = decimal_to_balanced_trits<TRITS_IN_TRYTE>(n);
    };

    int Tryte::to_int() const {
        int res = 0;
        int base = 1;
        for (size_t i = 0 ; i < TRITS_IN_TRYTE; i++ ) {
            res += static_cast<int>(trits_[i]) * base;
            base *= 3;
        }
        return res;
    }

    Tryte Tryte::negate() const {
        Tryte result;
        for (size_t i = 0; i < TRITS_IN_TRYTE; ++i) {
            if (trits_[i] == Trit::POSITIVE) {
                result.trits_[i] = Trit::NEGATIVE;
            } else if (trits_[i] == Trit::NEGATIVE) {
                result.trits_[i] = Trit::POSITIVE;
            } else {
                result.trits_[i] = Trit::ZERO;
            }
        }
        return result;
    }

    Trit Tryte::full_add(const Tryte& other, const Trit carry_in) {
        auto carry = carry_in;
        for (size_t i = 0; i < TRITS_IN_TRYTE; ++i) {
            // Need to change this as sum of 3 trits could only at max be -3 or 3.
            int sum = static_cast<int>(trits_[i]) + static_cast<int>(other.trits_[i]) + static_cast<int>(carry);
            carry = Trit::ZERO;
            if (sum > 1) {
                sum -= 3;
                carry = Trit::POSITIVE;
            } else if (sum < -1) {
                sum += 3;
                carry = Trit::NEGATIVE;
            }
            this->trits_[i] = static_cast<Trit>(sum);
        }
        // we could check carry here for overflow saving, but I feel allowing overflow feels more like normal computing.
        return carry;
    }

    auto Tryte::operator+=(const Tryte& rhs) -> Tryte& {
        return this->full_add(rhs), *this;
    }

    auto operator-(const Tryte &tryte) -> Tryte {
        return tryte.negate();
    }

    auto Tryte::operator-=(const Tryte &rhs) -> Tryte& {
        return this->full_add(-rhs), *this;
    }

    auto operator+(const Tryte& lhs, const Tryte& rhs) -> Tryte {
        Tryte result = lhs;
        result += rhs;
        return result;
    }

    std::ostream& operator<<(std::ostream& os, const Tryte& tryte) {
        os << "{ ";
        for (size_t i = 0; i < TRITS_IN_TRYTE; ++i) {
            os << static_cast<int>(tryte.trits_[i]) << (i < tryte.trits_.size() - 1 ? ", " : "");
        }
        os << " }";
        return os;
    }




}
