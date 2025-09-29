pub mod opcode;
pub mod instruction_set;

use thiserror::Error;

#[derive(Error, Debug)]
pub enum ArchError {
    #[error("invalid opcode: 0x{0:02X}")]
    InvalidOpcode(u8),

    #[error("value is out of range")]
    ValueOutOfRange(#[from] std::num::TryFromIntError),
}