//
// Created by jvigu on 9/7/2025.
//
#include <triode/types.h>

namespace triode::arch {

    Tryte::Tryte(const int n) {
        if (n < TRYTE_MIN || n > TRYTE_MAX) {
            throw std::invalid_argument("Invalid decimal value to put into one tryte.");
        }
        trits = decimal_to_balanced_trits<TRITS_IN_TRYTE>(n);
    };

    Tryte::Tryte(const std::initializer_list<int> list) {
        trits.fill(Trit::ZERO); // Start with a zeroed array
        size_t i = 0;
        // Copy elements from the list, but don't go past the end of our array
        for (int val : list) {
            if (i >= TRITS_IN_TRYTE) break;
            trits[i++] = static_cast<Trit>(val);
        }
    }

    int Tryte::to_int() const {
        int res = 0;
        int base = 1;
        for (size_t i = 0 ; i < TRITS_IN_TRYTE; i++ ) {
            res += static_cast<int>(trits[i]) * base;
            base *= 3;
        }
        return res;
    }

    std::pair<Tryte, Trit> full_add() {

    }

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
                trytes[i].trits[j] = trits[i * TRITS_IN_TRYTE + j];
            }
        }
    }

    int64_t Word::to_int() const {
        //inverse of the constructor oh yeah.
        int64_t res = 0;
        constexpr int base = pow3(TRITS_IN_TRYTE);
        int64_t place = 1;
        for (size_t i = 0; i < TRYTES_IN_WORD; i++) {
            res += place * trytes[i].to_int();
            place *= base;
        }
        return res;
    }

}
