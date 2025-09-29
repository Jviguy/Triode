use std::convert::Infallible;
use bternary::Word;
use crate::opcode::{InvalidOpCode, OpCode};

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Register(pub u8);

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Immediate(pub i64);


pub trait InstructionSet {
    fn opcode(&self) -> OpCode;
    fn rd(&self) -> Register;
    fn rs1(&self) -> Register;
    fn rs2(&self) -> Register;
    fn immediate12(&self) -> Immediate;
    fn immediate18(&self) -> Immediate;

    fn write_opcode(&mut self, opcode: OpCode) -> &mut Self;
    fn write_rd(&mut self, rd: Register) -> &mut Self;
    fn write_rs1(&mut self, rs1: Register) -> &mut Self;
    fn write_rs2(&mut self, rs2: Register) -> &mut Self;
    fn write_immediate12(&mut self, immediate12: Immediate) -> &mut Self;
    fn write_immediate18(&mut self, immediate18: Immediate) -> &mut Self;
}

impl InstructionSet for Word {
    fn opcode(&self) -> Result<OpCode, InvalidOpCode> {
        OpCode::try_from(self.read_trit_range(0,6))
    }

    fn rd(&self) -> Register {
        todo!()
    }

    fn rs1(&self) -> Register {
        todo!()
    }

    fn rs2(&self) -> Register {
        todo!()
    }

    fn immediate12(&self) -> Immediate {
        todo!()
    }

    fn immediate18(&self) -> Immediate {
        todo!()
    }

    fn write_opcode(&mut self, opcode: OpCode) -> &mut Self {
        todo!()
    }

    fn write_rd(&mut self, rd: Register) -> &mut Self {
        todo!()
    }

    fn write_rs1(&mut self, rs1: Register) -> &mut Self {
        todo!()
    }

    fn write_rs2(&mut self, rs2: Register) -> &mut Self {
        todo!()
    }

    fn write_immediate12(&mut self, immediate12: Immediate) -> &mut Self {
        todo!()
    }

    fn write_immediate18(&mut self, immediate18: Immediate) -> &mut Self {
        todo!()
    }
}