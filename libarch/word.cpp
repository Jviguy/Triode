//
// Created by jvigu on 9/12/2025.
//
#include <triode/word.h>
#include <triode/tryte.h>

namespace triode::arch {
    Word::Word(const int64_t n) {
        if (n == 0) {
            //default construction handles this already.
            return;
        }
        if (n < WORD_MIN || n > WORD_MAX) {
            throw std::invalid_argument("Invalid decimal value to put into one word.");
        }
        const auto trits = decimal_to_balanced_trits<TRITS_IN_WORD>(n);
        for (size_t i = 0; i < TRYTES_IN_WORD; i++) {
            for (size_t j = 0; j < TRITS_IN_TRYTE; j++) {
                trytes_[i].trits_[j] = trits[i * TRITS_IN_TRYTE + j];
            }
        }
    }

    int64_t Word::to_int() const {
        //inverse of the constructor oh yeah.
        int64_t res = 0;
        constexpr int base = pow3(TRITS_IN_TRYTE);
        int64_t place = 1;
        for (size_t i = 0; i < TRYTES_IN_WORD; i++) {
            res += place * trytes_[i].to_int();
            place *= base;
        }
        return res;
    }

    Trit Word::full_add(const Word& other, const Trit carry_in) {
        Trit carry = carry_in;
        for (size_t i = 0; i < TRYTES_IN_WORD; i++) {
            // add the other side's tryte and the carry to this tryte.
            carry = trytes_[i].full_add(other.trytes_[i], carry);
        }
        return carry;
    }

    Word Word::negate() const {
        Word res;
        for (size_t i = 0; i < TRYTES_IN_WORD; i++) {
            res.trytes_[i] = trytes_[i].negate();
        }
        return res;
    }

}