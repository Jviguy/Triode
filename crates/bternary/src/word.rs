use crate::balanced_int::{TernaryIntegerRepr, BalancedInt, ArithmeticTernaryInteger};

pub const TRYTES_IN_WORD: usize = 4;
const TRITS_IN_WORD: usize = crate::tryte::TRITS_IN_TRYTE * TRYTES_IN_WORD;

pub type Word = BalancedInt<TRITS_IN_WORD>;

impl TernaryIntegerRepr for Word {
    type Int = i64;
}

impl ArithmeticTernaryInteger for Word {}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::Trit;

    const TESTING_TRITS: [Trit; 24] = [Trit::Neg, Trit::Zero, Trit::Pos, Trit::Neg, Trit::Pos, Trit::Zero,
    Trit::Neg, Trit::Zero, Trit::Pos, Trit::Neg, Trit::Pos, Trit::Zero,
    Trit::Neg, Trit::Zero, Trit::Pos, Trit::Neg, Trit::Pos, Trit::Zero,
    Trit::Neg, Trit::Zero, Trit::Pos, Trit::Neg, Trit::Pos, Trit::Zero];

    #[test]
    fn test_word_zero() {
        let word = Word::zero();
        assert_eq!(word, Word::new([Trit::Zero; TRITS_IN_WORD]));
    }

    #[test]
    fn test_word_from_trit() {
        let word = Word::from(Trit::Pos);
        let mut expected = [Trit::Zero; TRITS_IN_WORD];
        expected[0] = Trit::Pos;
        assert_eq!(word, Word::new(expected));
    }

    #[test]
    fn test_word_from_trits() {
        let word = Word::from(TESTING_TRITS);
        assert_eq!(word, Word::new(TESTING_TRITS));
    }

    #[test]
    fn test_word_display() {
        let word = Word::from(TESTING_TRITS);
        let display = format!("{}", word);
        let expected = "01T10T01T10T01T10T01T10T";
        assert_eq!(display, expected);
    }
}