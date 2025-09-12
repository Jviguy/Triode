#ifndef TRIODE_WORD_H
#define TRIODE_WORD_H

#include <triode/tryte.h>

namespace triode::arch {
    constexpr int TRYTES_IN_WORD = 4;
    constexpr int TRITS_IN_WORD = TRITS_IN_TRYTE * TRYTES_IN_WORD;
    constexpr int64_t WORD_MAX = (pow3(TRITS_IN_WORD) - 1) / 2;
    constexpr int64_t WORD_MIN = -WORD_MAX;

    class DoubleWord;

    class Word {
    public:
        std::array<Tryte, TRYTES_IN_WORD> trytes_{};
        Word() = default;
        // Constructs as Word from an array of trytes.
        Word(const std::array<Tryte, TRYTES_IN_WORD> &trytes) noexcept : trytes_(trytes) {}; //NOLINT
        // CConstructs a Word from an int64_t, throws if out of range.
        Word(int64_t n); //NOLINT

        // all default memory management because std::array fits this perfectly
        // and just works.
        Word(const Word& other) = default;
        Word& operator=(const Word& other) = default;
        Word(Word&& other) noexcept = default;
        Word& operator=(Word&& other) noexcept = default;
        ~Word() = default;


        [[nodiscard]] int64_t to_int() const;

        Trit full_add(const Word& other, Trit carry_in = Trit::ZERO);

        [[nodiscard]] Word negate() const;
    };

    class DoubleWord : public Word {
    public:
        std::array<Tryte, TRYTES_IN_WORD * 2> trytes{};
    };
}

#endif //TRIODE_WORD_H