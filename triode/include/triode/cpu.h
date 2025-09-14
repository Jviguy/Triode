//
// Created by jvigu on 9/12/2025.
//

#ifndef TRIODE_CPU_H
#define TRIODE_CPU_H
#include <array>

#include "memory.h"
#include <triode/arch/word.h>
// for now.

namespace triode {

    struct EmulatorTestBench;

    class CPU {
    public:
        CPU() = default;

        void cycle(Memory& memory);

        void reset();

        [[nodiscard]] bool is_halted() const;

    private:
        arch::Word program_counter_;
        bool halted_{};

        // R0 - R26 general registers. R26 being Stack pointer.
        std::array<arch::Word, 27> registers_;

        struct StatusRegister {
            arch::Trit result_flag = arch::Trit::ZERO;
            arch::Trit carry_flag = arch::Trit::ZERO;
        } status_register_;


        void op_halt();
        void op_load(const arch::Word &instruction, const Memory& memory);
        void op_store(const arch::Word &instruction, Memory& memory) const;


        void set_result_flag(const arch::Word &result);
        void op_add(const arch::Word &instruction, Memory& memory);
        void op_sub(const arch::Word &instruction, Memory& memory);

        void op_addi(const arch::Word &instruction, Memory& memory);
        void op_subi(const arch::Word &instruction, Memory& memory);

        void op_out(const arch::Word &instruction, Memory& memory) const;

        void execute(const arch::Word &instruction, Memory& memory);

        friend struct EmulatorTestBench;

    };
}

#endif //TRIODE_CPU_H