//
// Created by jvigu on 9/12/2025.
//

#ifndef TRIODE_INSTRUCTIONS_H
#define TRIODE_INSTRUCTIONS_H
#include <cstdint>

namespace triode::arch {
    enum class OpCode : uint8_t {
        NOP,
        HALT,

        LOAD,
        STORE,
        MOV,

        ADD,
        SUB,
        //MUL,
        //DIV,

        IN,
        OUT,

        JMP,
        JEQ,
        JNE,
        JGT,
        JLT,
        JGE,
        JLE,
    };
}
#endif //TRIODE_INSTRUCTIONS_H