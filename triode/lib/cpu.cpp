//
// Created by jvigu on 9/12/2025.
//

#include <triode/cpu.h>
#include <triode/arch/instructions.h>

namespace triode {

    void CPU::cycle(Memory &memory) {
        const arch::Word instruction = memory.read_word(program_counter_.to_address());
        program_counter_ += arch::TRYTES_IN_WORD;
        execute(instruction, memory);
    }

    void CPU::reset() {
        program_counter_ = arch::Word();
        halted_ = false;
        // clear registers
        // just make a new set of these its almost the same as just setting 0 to each register.
        registers_ = std::array<arch::Word, 27>{};
        // same here just make a new status register will default all to 0.
        status_register_ = StatusRegister();
    }

    bool CPU::is_halted() const {
        return halted_;
    }

    void CPU::op_halt() {
        halted_ = true;
    }

    void CPU::op_load(const arch::Word &instruction, const Memory &memory) {
        // data register.
        const auto rd = instruction.rd();
        // base register.
        const auto rs = instruction.rs1();
        // offset
        const auto offset = instruction.immediate12();
        // RD is basically the destination.
        // RS a register containing a memory address
        // offset is an immediate value that should be added to the the memory address in RS.
        // Read from RS + AO put into RD
        registers_[rd] = memory.read_word((registers_[rs] + offset).to_address());
    }

    void CPU::op_store(const arch::Word &instruction, Memory &memory) const {
        const auto rd = instruction.rd();
        const auto rs = instruction.rs1();
        const auto offset = instruction.immediate12();

        memory.write_word((registers_[rd] + offset).to_address(), registers_[rs]);
    }

    void CPU::set_result_flag(const arch::Word &result) {
        if (result == arch::Word(0)) {
            status_register_.result_flag = arch::Trit::ZERO;
        } else if (result > arch::Word(0)) {
            status_register_.result_flag = arch::Trit::POSITIVE;
        } else {
            status_register_.result_flag = arch::Trit::NEGATIVE;
        }
    }

    void CPU::op_add(const arch::Word &instruction, Memory &memory) {
        const auto rd = instruction.rd();
        const auto rs1 = instruction.rs1();
        const auto rs2 = instruction.rs2();

        // copy value of rs1 into the destination.
        registers_[rd] = registers_[rs1];
        // we are gonna then just use .full_add which is in place to do rs1 + rs2 in place.
        status_register_.carry_flag = registers_[rd].full_add(registers_[rs2]);
        set_result_flag(registers_[rd]);
    }

    void CPU::op_sub(const arch::Word &instruction, Memory &memory) {
        const auto rd = instruction.rd();
        const auto rs1 = instruction.rs1();
        const auto rs2 = instruction.rs2();

        registers_[rd] = registers_[rs1];
        status_register_.carry_flag = registers_[rd].full_add(registers_[rs2].negate());
        set_result_flag(registers_[rd]);
    }

    void CPU::op_addi(const arch::Word &instruction, Memory &memory) {
        const auto rd = instruction.rd();
        const auto rs1 = instruction.rs1();
        const auto imm = instruction.immediate12();

        registers_[rd] = registers_[rs1];
        status_register_.carry_flag = registers_[rd].full_add(imm);
        set_result_flag(registers_[rd]);
    }

    void CPU::op_subi(const arch::Word &instruction, Memory &memory) {
        const auto rd = instruction.rd();
        const auto rs1 = instruction.rs1();
        const auto imm = instruction.immediate12();

        registers_[rd] = registers_[rs1];
        status_register_.carry_flag = registers_[rd].full_add(imm.negate());
        set_result_flag(registers_[rd]);
    }

    void CPU::op_out(const arch::Word &instruction, Memory &memory) const {
        const auto rd = instruction.rd();
        const auto rs1 = instruction.rs1();

        if (registers_[rd] == arch::Word(0)) {
            std::cout << registers_[rs1].to_address() << std::endl;
        }
    }


    void CPU::execute(const arch::Word &instruction, Memory &memory) {
        switch (instruction.opcode()) {
            case arch::OpCode::HALT:
                op_halt();
                break;
            case arch::OpCode::LOAD:
                op_load(instruction, memory);
                break;
            case arch::OpCode::STORE:
                op_store(instruction, memory);
                break;
            case arch::OpCode::ADD:
                op_add(instruction, memory);
                break;
            case arch::OpCode::SUB:
                op_sub(instruction, memory);
                break;
            case arch::OpCode::ADDI:
                op_addi(instruction, memory);
                break;
            case arch::OpCode::SUBI:
                op_subi(instruction, memory);
                break;
            case arch::OpCode::IN:
                break;
            case arch::OpCode::OUT:
                op_out(instruction, memory);
                break;
            case arch::OpCode::JMP:
                break;
            case arch::OpCode::JEQ:
                break;
            case arch::OpCode::JNE:
                break;
            case arch::OpCode::JGT:
                break;
            case arch::OpCode::JLT:
                break;
            case arch::OpCode::AND:
                break;
            case arch::OpCode::OR:
                break;
            case arch::OpCode::NOT:
                break;
            case arch::OpCode::SHL:
                break;
            case arch::OpCode::SHR:
                break;
            case arch::OpCode::CMP:
                break;
            case arch::OpCode::CMPI:
                break;
            case arch::OpCode::PUSH:
                break;
            case arch::OpCode::POP:
                break;
            case arch::OpCode::CALL:
                break;
            case arch::OpCode::RET:
                break;
            case arch::OpCode::NOP:
            default:
                // it's NOP.
                break;
        }
    }
}
