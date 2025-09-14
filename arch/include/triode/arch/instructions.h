//
// Created by jvigu on 9/12/2025.
//

#ifndef TRIODE_INSTRUCTIONS_H
#define TRIODE_INSTRUCTIONS_H
#include <cstdint>

namespace triode::arch {
    // A RISC ish instruction set aka the not a lot lel.
    enum class OpCode : uint8_t {
        // Control
        NOP,    // No operation
        HALT,   // Halt execution

        // Memory
        LOAD,   // Load word from memory into register
        STORE,  // Store word from register into memory

        // Arithmetic
        ADD,    // Add two registers
        SUB,    // Subtract two registers
        ADDI,   // Add immediate value to register
        SUBI,   // Subtract immediate value from register

        // Logical & Shift
        AND,    // Logical AND of two registers
        OR,     // Logical OR of two registers
        NOT,    // Logical NOT of a register
        SHL,    // Shift Left (Logical)
        SHR,    // Shift Right (Logical/Arithmetic)

        // Comparison
        CMP,    // Compare two registers
        CMPI,   // Compare register with immediate

        // I/O
        IN,     // Read from input device into register
        OUT,    // Write from register to output device

        // Jumps & Subroutines
        JMP,    // Unconditional jump
        JEQ,    // Jump if equal (Zero flag is set)
        JNE,    // Jump if not equal (Zero flag is clear)
        JGT,    // Jump if greater than
        JLT,    // Jump if less than
        PUSH,   // Push register onto stack
        POP,    // Pop from stack into register
        CALL,   // Call subroutine
        RET,    // Return from subroutine
    };
}
#endif //TRIODE_INSTRUCTIONS_H