//
// Created by jvigu on 9/12/2025.
//
#include <triode/arch/word.h>
#include <triode/arch/tryte.h>

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

    Word::Word(const Tryte &tryte) {
        // Little endian so just set first tryte to this.
        trytes_[0] = tryte;
    }

    Word::Word(const Tryte &tryte1, const Tryte &tryte2, const Tryte &tryte3, const Tryte &tryte4) {
        trytes_[0] = tryte1;
        trytes_[1] = tryte2;
        trytes_[2] = tryte3;
        trytes_[3] = tryte4;
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

    size_t Word::to_address() const {
        return static_cast<size_t>(to_int());
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

    OpCode Word::opcode() const {
        // OpCode is 6 trits (0-5)
        return static_cast<OpCode>(get_trit_range(0, 6) + 13);
    }

    uint8_t Word::rd() const {
        // Rd is 3 trits (6-8)
        return static_cast<uint8_t>(get_trit_range(6, 3) + 13);
    }

    uint8_t Word::rs1() const {
        // Rs1 is 3 trits (9-11)
        return static_cast<uint8_t>(get_trit_range(9, 3) + 13);
    }

    uint8_t Word::rs2() const {
        // Rs2 is 3 trits (12-14)
        return static_cast<uint8_t>(get_trit_range(12, 3) + 13);
    }

    Word Word::immediate12() const {
        // Immediate is 12 trits (12-23)
        return Word(get_trit_range(12, 12));
    }

    Word Word::immediate18() const {
        // Address offset is 18 trits (6-23)
        return Word(get_trit_range(6, 18));
    }


    Word& Word::set_opcode(OpCode op) {
        // OpCode is 6 trits (0-5)
        set_trit_range(0, 6, static_cast<int64_t>(op) - 13);
        return *this;
    }

    Word& Word::set_rd(const uint8_t reg_index) {
        // Rd is 3 trits (6-8)
        set_trit_range(6, 3, reg_index - 13);
        return *this;
    }

    Word& Word::set_rs1(const uint8_t reg_index) {
        // Rs1 is 3 trits (9-11)
        set_trit_range(9, 3, reg_index - 13);
        return *this;
    }

    Word& Word::set_rs2(const uint8_t reg_index) {
        // Rs2 is 3 trits (12-14)
        set_trit_range(12, 3, reg_index - 13);
        return *this;
    }

    Word& Word::set_immediate12(const int32_t value) {
        // Immediate is 12 trits (12-23)
        set_trit_range(12, 12, value);
        return *this;
    }

    Word& Word::set_immediate18(const int32_t value) {
        // Address offset is 18 trits (6-23)
        set_trit_range(6, 18, value);
        return *this;
    }

    auto Word::operator<=>(const Word &rhs) const -> std::strong_ordering {
        for (int i = TRYTES_IN_WORD - 1; i >= 0; --i) {
            // If the trytes are not equal, we have found our answer
            // and can return immediately.
            if (const std::strong_ordering result = trytes_[i] <=> rhs.trytes_[i];
                result != std::strong_ordering::equal) {
                return result;
            }
        }

        return std::strong_ordering::equal;
    }

    bool Word::operator==(const Word &rhs) const {
        return (*this <=> rhs) == std::strong_ordering::equal;
    }

    bool Word::operator!=(const Word &rhs) const {
        return (*this <=> rhs) != std::strong_ordering::equal;
    }

    auto Word::operator+=(const Word& rhs) -> Word& {
        return full_add(rhs), *this;
    }

    auto Word::operator-=(const Word& rhs) -> Word & {
        return *this += -rhs, *this;
    }

    auto Word::operator+=(const int64_t rhs) -> Word & {
        return *this += Word(rhs), *this;
    }

    auto operator+(const Word &lhs, const Word &rhs) -> Word {
        Word res(lhs);
        res += rhs;
        return res;
    }

    Word operator+(const Word &lhs, const Tryte &ao) {
        Word lhs1(lhs);
        // TODO: Maybe change this but hacky fix that works just a little bit of copying.
        return lhs1 += Word(ao);
    }


    auto operator-(const Word& word) -> Word {
        return word.negate();
    }

    auto operator-(const Word &lhs, const Word &rhs) -> Word {
        Word res(lhs);
        res += -rhs;
        return res;
    }

    auto operator<<(std::ostream &os, const Word &word) -> std::ostream & {
        os << "{ ";
        for (size_t i = 0; i < TRYTES_IN_WORD; ++i) {
            os << word.trytes_[i] << (i < word.trytes_.size() - 1 ? ", " : "");
        }
        os << " }";
        return os;
    }
}
