use bternary::balanced_int::{TernaryIntegerRepr, BalancedInt, ArithmeticTernaryInteger};

pub const TRYTES_IN_WORD: usize = 4;
const TRITS_IN_WORD: usize = crate::tryte::TRITS_IN_TRYTE * TRYTES_IN_WORD;

pub struct Word(pub BalancedInt<TRITS_IN_WORD>);

impl TernaryIntegerRepr for Word {
    type Int = i64;
}

impl ArithmeticTernaryInteger for Word {}

#[cfg(test)]
mod tests {
    use super::*;
    use bternary::Trit;

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
        let trits = [Trit::Neg, Trit::Zero, Trit::Pos, Trit::Neg, Trit::Pos, Trit::Zero,
                     Trit::Neg, Trit::Zero, Trit::Pos, Trit::Neg, Trit::Pos, Trit::Zero,
                     Trit::Neg, Trit::Zero, Trit::Pos, Trit::Neg, Trit::Pos, Trit::Zero,
                     Trit::Neg, Trit::Zero, Trit::Pos, Trit::Neg, Trit::Pos, Trit::Zero];
        let word = Word::from(trits);
        assert_eq!(word, Word::new(trits));
    }

    #[test]
    fn test_word_display() {
        let trits = [Trit::Neg, Trit::Zero, Trit::Pos, Trit::Neg, Trit::Pos, Trit::Zero,
                     Trit::Neg, Trit::Zero, Trit::Pos, Trit::Neg, Trit::Pos, Trit::Zero,
                     Trit::Neg, Trit::Zero, Trit::Pos, Trit::Neg, Trit::Pos, Trit::Zero,
                     Trit::Neg, Trit::Zero, Trit::Pos, Trit::Neg, Trit::Pos, Trit::Zero];
        let word = Word::from(trits);
        let display = format!("{}", word);
        let expected = "BalancedInt[Neg, Zero, Pos, Neg, Pos, Zero, Neg, Zero, Pos, Neg, Pos, Zero, Neg, Zero, Pos, Neg, Pos, Zero, Neg, Zero, Pos, Neg, Pos, Zero]";
        assert_eq!(display, expected);
    }
}