use crate::balanced_int::{TernaryIntegerRepr, BalancedInt};

pub const TRITS_IN_TRYTE: usize = 6;

pub type Tryte = BalancedInt<TRITS_IN_TRYTE>;

impl TernaryIntegerRepr for Tryte {
    type Int = i16;
}