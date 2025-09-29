use std::fmt::{Display, Formatter};

#[repr(u8)]
pub enum OpCode {
    NOP = 0x00,
}

#[derive(Debug)]
pub struct InvalidOpCode(u8);

impl Display for InvalidOpCode {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        f.write_fmt(format_args!("Invalid OpCode provided: {:x}", self.0))
    }
}

impl std::error::Error for InvalidOpCode {}

impl TryFrom<u8> for OpCode {
    type Error = InvalidOpCode;

    fn try_from(value: u8) -> Result<Self, Self::Error> {
        match value {
            0x00 => Ok(OpCode::NOP),
            _ => Err(InvalidOpCode(value))
        }
    }
}