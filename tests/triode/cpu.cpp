//
// Created by jvigu on 9/13/2025.
//
#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <triode/arch/word.h>
#include <triode/cpu.h>

#include "triode/emulator.h"

using namespace triode;
using namespace triode::arch;

namespace triode {
    /**
     * @struct EmulatorTestBench
     * @brief A test fixture providing a clean emulator state for each test case.
     *
     * This struct leverages its 'friend' status to access the private internals
     * of the Emulator and CPU classes for precise test setup and verification.
     */
    struct EmulatorTestBench {
        Emulator emulator;
        CPU& cpu;
        Memory& memory;

        explicit EmulatorTestBench(const size_t memory_size = 256)
            : emulator(memory_size),
              cpu(emulator.get_cpu()),
              memory(emulator.get_memory())
        {}

        void set_register(const int index, const Word& value) const {
            cpu.registers_[index] = value;
        }

        [[nodiscard]] Word get_register(const int index) const {
            return cpu.registers_[index];
        }

        [[nodiscard]] Word get_program_counter_() const {
            return cpu.program_counter_;
        }

        void stage_instruction(const Word& instruction) const {
            cpu.program_counter_ = Word(0);
            memory.write_word(0, instruction);
        }

        void cycle() const {
            cpu.cycle(memory);
        }

        void stage_and_cycle(const Word& instruction) const {
            stage_instruction(instruction);
            cycle();
        }

        [[nodiscard]] CPU::StatusRegister get_status_register() const {
            return cpu.status_register_;
        }
    };
}

TEST_CASE("CPU executes STORE instruction correctly", "[cpu][memory]") {
    const EmulatorTestBench bench;
    const Word test_value(1337);
    bench.set_register(1, test_value); // Set r1 = 1337 using the helper

    Word instruction(OpCode::STORE);
    instruction.set_rs1(1);
    instruction.set_rd(0);
    instruction.set_immediate12(20);
    bench.stage_and_cycle(instruction);

    REQUIRE(bench.memory.read_word(20) == test_value);
    REQUIRE(bench.get_program_counter_() == Word(4)); // Verify PC using the helper
}

TEST_CASE("CPU executes LOAD instruction correctly", "[cpu][memory]") {
    const EmulatorTestBench bench;
    const Word test_value(42);
    bench.memory.write_word(40, test_value);
    bench.set_register(2, Word(0)); // Clear destination r2 using the helper

    Word instruction(OpCode::LOAD);
    instruction.set_rd(2);
    instruction.set_rs1(0);
    instruction.set_immediate12(40);
    bench.stage_and_cycle(instruction);

    REQUIRE(bench.get_register(2) == test_value); // Verify r2 using the helper
    REQUIRE(bench.get_program_counter_() == Word(4));
}

TEST_CASE("CPU Arithmetic Instructions", "[cpu][arithmetic]") {
    EmulatorTestBench bench;

    // A helper lambda to check all status flags at once
    auto check_flags = [&](Trit result, Trit carry) {
        REQUIRE(bench.get_status_register().result_flag == result);
        REQUIRE(bench.get_status_register().carry_flag == carry);
    };

    SECTION("ADD - Add Registers") {
        bench.set_register(1, Word(100));
        bench.set_register(2, Word(50));

        SECTION("Positive result") {
            // R3 = R1 (100) + R2 (50) = 150
            Word instruction;
            instruction.set_opcode(OpCode::ADD).set_rd(3).set_rs1(1).set_rs2(2);
            bench.stage_and_cycle(instruction);

            REQUIRE(bench.get_register(3) == Word(150));
            check_flags(Trit::POSITIVE, Trit::ZERO);
        }

        SECTION("Result is zero") {
            // R3 = R1 (100) + R2 (-100) = 0
            bench.set_register(2, Word(-100));
            Word instruction;
            instruction.set_opcode(OpCode::ADD).set_rd(3).set_rs1(1).set_rs2(2);
            bench.stage_and_cycle(instruction);

            REQUIRE(bench.get_register(3) == Word(0));
            check_flags(Trit::ZERO, Trit::ZERO);
        }

        SECTION("Negative result") {
            // R3 = R1 (-100) + R2 (-50) = -150
            bench.set_register(1, Word(-100));
            bench.set_register(2, Word(-50));
            Word instruction;
            instruction.set_opcode(OpCode::ADD).set_rd(3).set_rs1(1).set_rs2(2);
            bench.stage_and_cycle(instruction);

            REQUIRE(bench.get_register(3) == Word(-150));
            check_flags(Trit::NEGATIVE, Trit::ZERO);
        }

        SECTION("Overflow (carry) result") {
            // R3 = R1 (WORD_MAX) + R2 (1) -> wraps to WORD_MIN
            bench.set_register(1, Word(WORD_MAX));
            bench.set_register(2, Word(1));
            Word instruction;
            instruction.set_opcode(OpCode::ADD).set_rd(3).set_rs1(1).set_rs2(2);
            bench.stage_and_cycle(instruction);

            REQUIRE(bench.get_register(3) == Word(WORD_MIN));
            check_flags(Trit::NEGATIVE, Trit::POSITIVE);
        }
    }

    SECTION("ADDI - Add Immediate") {
        bench.set_register(1, Word(100));

        SECTION("Positive result") {
            // R2 = R1 (100) + 50 = 150
            Word instruction;
            instruction.set_opcode(OpCode::ADDI).set_rd(2).set_rs1(1).set_immediate12(50);
            bench.stage_and_cycle(instruction);

            REQUIRE(bench.get_register(2) == Word(150));
            check_flags(Trit::POSITIVE, Trit::ZERO);
        }
    }

    SECTION("SUB - Subtract Registers") {
        bench.set_register(1, Word(100));
        bench.set_register(2, Word(50));

        SECTION("Positive result") {
            // R3 = R1 (100) - R2 (50) = 50
            Word instruction;
            instruction.set_opcode(OpCode::SUB).set_rd(3).set_rs1(1).set_rs2(2);
            bench.stage_and_cycle(instruction);

            REQUIRE(bench.get_register(3) == Word(50));
            check_flags(Trit::POSITIVE, Trit::ZERO);
        }

        SECTION("Result is zero") {
            // R3 = R1 (100) - R2 (100) = 0
            bench.set_register(2, Word(100));
            Word instruction;
            instruction.set_opcode(OpCode::SUB).set_rd(3).set_rs1(1).set_rs2(2);
            bench.stage_and_cycle(instruction);

            REQUIRE(bench.get_register(3) == Word(0));
            check_flags(Trit::ZERO, Trit::ZERO);
        }

        SECTION("Negative result") {
            // R3 = R1 (50) - R2 (100) = -50
            bench.set_register(1, Word(50));
            bench.set_register(2, Word(100));
            Word instruction;
            instruction.set_opcode(OpCode::SUB).set_rd(3).set_rs1(1).set_rs2(2);
            bench.stage_and_cycle(instruction);

            REQUIRE(bench.get_register(3) == Word(-50));
            check_flags(Trit::NEGATIVE, Trit::ZERO);
        }

        SECTION("Underflow (borrow/carry) result") {
            // R3 = R1 (WORD_MIN) - R2 (1) -> wraps to WORD_MAX
            bench.set_register(1, Word(WORD_MIN));
            bench.set_register(2, Word(1));
            Word instruction;
            instruction.set_opcode(OpCode::SUB).set_rd(3).set_rs1(1).set_rs2(2);
            bench.stage_and_cycle(instruction);

            REQUIRE(bench.get_register(3) == Word(WORD_MAX));
            // since we subtracted from WORD_MIN we should get a positive result and a negative carry.
            check_flags(Trit::POSITIVE, Trit::NEGATIVE);
        }
    }

    SECTION("SUBI - Subtract Immediate") {
        bench.set_register(1, Word(100));

        SECTION("Negative result") {
            // R2 = R1 (100) - 150 = -50
            Word instruction;
            instruction.set_opcode(OpCode::SUBI).set_rd(2).set_rs1(1).set_immediate12(150);
            bench.stage_and_cycle(instruction);

            REQUIRE(bench.get_register(2) == Word(-50));
            check_flags(Trit::NEGATIVE, Trit::ZERO);
        }
    }
}