#ifndef TRIODE_WORD_H
#define TRIODE_WORD_H

#include <memory>
#include <triode/arch/tryte.h>

#include <triode/arch/instructions.h>

namespace triode::arch {
    constexpr int TRYTES_IN_WORD = 4;
    constexpr int TRITS_IN_WORD = TRITS_IN_TRYTE * TRYTES_IN_WORD;
    constexpr int64_t WORD_MAX = (pow3(TRITS_IN_WORD) - 1) / 2;
    constexpr int64_t WORD_MIN = -WORD_MAX;

    using DoubleWord = std::array<Tryte, 8>;

    class Word : public TritwiseOperations<Word, TRYTES_IN_WORD, TRITS_IN_TRYTE>  {
    public:
        std::array<Tryte, TRYTES_IN_WORD> trytes_{};
        Word() = default;
        // Constructs as Word from an array of trytes.
        explicit Word(const std::array<Tryte, TRYTES_IN_WORD> &trytes) noexcept : trytes_(trytes) {}
        // Constructs a Word from an int64_t, throws if out of range.
        explicit Word(int64_t n);
        // Constructs a Word from a Tryte useful for loading smaller things into a register.
        explicit Word(const Tryte& tryte);
        explicit Word(const Tryte& tryte1, const Tryte& tryte2, const Tryte& tryte3, const Tryte& tryte4);
        explicit Word(const OpCode& opcode) { set_opcode(opcode); }
        // Maybe a move constructor but it's the same as copying here as its just std::arrays which I think only really
        // copy as it's not on the heap. So can't swap ptrs unless its done nastily. I don't know stack ptr swapping
        // sounds weird.

        // all default memory management because std::array fits this perfectly
        // and just works.
        Word(const Word& other) = default;
        Word& operator=(const Word& other) = default;
        Word(Word&& other) noexcept = default;
        Word& operator=(Word&& other) noexcept = default;
        ~Word() = default;


        [[nodiscard]] int64_t to_int() const;

        [[nodiscard]] size_t to_address() const;

        Trit full_add(const Word& other, Trit carry_in = Trit::ZERO);

        [[nodiscard]] Word negate() const;

        /**
         * @brief Extracts trits 0-5 and casts to the OpCode enum.
         */
        [[nodiscard]] OpCode opcode() const;

        /**
         * @brief Extracts trits 6-8 for the destination register index (Rd).
         */
        [[nodiscard]] uint8_t rd() const;

        /**
         * @brief Extracts trits 9-11 for the first source register index (Rs1).
         */
        [[nodiscard]] uint8_t rs1() const;

        /**
         * @brief Extracts trits 12-14 for the second source register index (Rs2).
         */
        [[nodiscard]] uint8_t rs2() const;

        /**
         * @brief Extracts the 12-trit immediate value from I-Type and S-Type instructions.
         */
        [[nodiscard]] Word immediate12() const;

        /**
         * @brief Extracts the 18-trit address offset from J-Type instructions.
         */
        [[nodiscard]] Word immediate18() const;


        // --- SETTERS (ENCODING) ---

        /**
         * @brief Sets trits 0-5 from an OpCode.
         */
        Word& set_opcode(OpCode op);

        /**
         * @brief Sets trits 6-8 from a register index (Rd).
         */
        Word& set_rd(uint8_t reg_index);

        /**
         * @brief Sets trits 9-11 from a register index (Rs1).
         */
        Word& set_rs1(uint8_t reg_index);

        /**
         * @brief Sets trits 12-14 from a register index (Rs2).
         */
        Word& set_rs2(uint8_t reg_index);

        /**
         * @brief Sets the 12-trit immediate value for I-Type and S-Type instructions.
         */
        Word& set_immediate12(int32_t value);

        /**
         * @brief Sets the 18-trit address offset for J-Type instructions.
         */
        Word& set_immediate18(int32_t value);

        [[nodiscard]] auto operator<=>(const Word& rhs) const -> std::strong_ordering;
        [[nodiscard]] auto operator==(const Word &rhs) const -> bool;
        [[nodiscard]] auto operator!=(const Word &rhs) const -> bool;

        auto operator+=(const Word& rhs) -> Word&;
        auto operator-=(const Word& rhs) -> Word&;

        auto operator+=(int64_t rhs) -> Word&;
    };

    auto operator-(const Word& word) -> Word;
    auto operator+(const Word& lhs, const Word& rhs) -> Word;
    auto operator+(const Word& lhs, const Tryte & ao) -> Word;
    auto operator-(const Word& lhs, const Word& rhs) -> Word;

    auto operator<<(std::ostream& os, const Word& word) -> std::ostream&;
}

#endif //TRIODE_WORD_H