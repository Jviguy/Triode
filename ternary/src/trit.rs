use std::ops::{Mul, Neg};

/// Balanced Trit, a ternary digit with values -1, 0, and 1.
/// We use `i8` as the underlying type to represent the three states.
/// This is useful for speed wise but wastes a good bit of memory.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(i8)]
pub enum Trit {
    Neg = -1,
    Zero = 0,
    Pos = 1,
}

impl Trit {
    /// Full adder for balanced trits.
    /// Returns a tuple of (result, carry).
    /// The result is the sum of `self`, `other`, and `carry_in`.
    /// The carry is -1, 0, or 1 depending on the overflow.
    /// For example:
    /// - `Trit::Pos.full_add(Trit::Pos, Trit::Pos)` returns `(Trit::Zero, Trit::Pos)` because 1 + 1 + 1 = 3, which is 0 with a carry of 1.
    pub fn full_add(&self, other: Trit, carry_in: Trit) -> (Trit, Trit) {
        let sum = (*self as i8) + (other as i8) + (carry_in as i8);
        let carry = (sum as f32 / 3.0).round() as i8;
        let result = sum - (carry * 3);

        (Trit::try_from(result).unwrap(), Trit::try_from(carry).unwrap())
    }

    pub fn multiply(&self, other: Trit) -> Trit {
        Trit::try_from((*self as i8) * (other as i8)).unwrap()
    }

    pub fn negate(&self) -> Trit {
        match self {
            Trit::Neg => Trit::Pos,
            Trit::Zero => Trit::Zero,
            Trit::Pos => Trit::Neg,
        }
    }
}

impl Neg for Trit {
    type Output = Trit;

    fn neg(self) -> Trit {
        self.negate()
    }
}

impl Mul for Trit {
    type Output = Trit;

    fn mul(self, rhs: Trit) -> Trit {
        self.multiply(rhs)
    }
}

#[derive(Debug, PartialEq, Eq)]
pub struct InvalidTritValueError(i8);

impl std::fmt::Display for InvalidTritValueError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "Invalid value for Trit: {}", self.0)
    }
}

impl std::error::Error for InvalidTritValueError {}

impl TryFrom<i8> for Trit {
    type Error = InvalidTritValueError;

    fn try_from(value: i8) -> Result<Self, Self::Error> {
        match value {
            -1 => Ok(Trit::Neg),
             0 => Ok(Trit::Zero),
             1 => Ok(Trit::Pos),
             _ => Err(InvalidTritValueError(value))
        }
    }
}

// --- Unit Tests ---
// The test module ensures our logic is correct for all cases.
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_full_add_exhaustive() {
        let trits = [Trit::Neg, Trit::Zero, Trit::Pos];
        // Test all 3*3*3 = 27 possible combinations
        for &a in &trits {
            for &b in &trits {
                for &c in &trits {
                    let sum = a as i8 + b as i8 + c as i8;
                    let (result, carry) = a.full_add(b, c);
                    let check_sum = result as i8 + (carry as i8 * 3);
                    assert_eq!(sum, check_sum, "Failed for inputs: {:?}, {:?}, {:?}", a, b, c);
                }
            }
        }

        // Spot check a few key cases for clarity
        // sum = -3 -> (result: 0, carry: -1)
        assert_eq!(Trit::Neg.full_add(Trit::Neg, Trit::Neg), (Trit::Zero, Trit::Neg));
        // sum = 2 -> (result: -1, carry: 1)
        assert_eq!(Trit::Pos.full_add(Trit::Pos, Trit::Zero), (Trit::Neg, Trit::Pos));
        // sum = 0 -> (result: 0, carry: 0)
        assert_eq!(Trit::Pos.full_add(Trit::Neg, Trit::Zero), (Trit::Zero, Trit::Zero));
    }

    #[test]
    fn test_negate_and_neg_op() {
        // Test the inherent method
        assert_eq!(Trit::Pos.negate(), Trit::Neg);
        assert_eq!(Trit::Zero.negate(), Trit::Zero);
        assert_eq!(Trit::Neg.negate(), Trit::Pos);

        // Test the `-` operator
        assert_eq!(-Trit::Pos, Trit::Neg);
        assert_eq!(-Trit::Zero, Trit::Zero);
        assert_eq!(-Trit::Neg, Trit::Pos);
    }

    #[test]
    fn test_multiply_and_mul_op() {
        // Test the inherent method
        assert_eq!(Trit::Pos.multiply(Trit::Pos), Trit::Pos);
        assert_eq!(Trit::Pos.multiply(Trit::Neg), Trit::Neg);
        assert_eq!(Trit::Neg.multiply(Trit::Pos), Trit::Neg);
        assert_eq!(Trit::Neg.multiply(Trit::Neg), Trit::Pos);
        assert_eq!(Trit::Pos.multiply(Trit::Zero), Trit::Zero);
        assert_eq!(Trit::Zero.multiply(Trit::Neg), Trit::Zero);

        // Test the `*` operator
        assert_eq!(Trit::Pos * Trit::Pos, Trit::Pos);
        assert_eq!(Trit::Neg * Trit::Pos, Trit::Neg);
        assert_eq!(Trit::Zero * Trit::Pos, Trit::Zero);
    }

    #[test]
    fn test_try_from_i8() {
        // Valid cases
        assert_eq!(Trit::try_from(1), Ok(Trit::Pos));
        assert_eq!(Trit::try_from(0), Ok(Trit::Zero));
        assert_eq!(Trit::try_from(-1), Ok(Trit::Neg));

        // Invalid cases
        assert_eq!(Trit::try_from(2), Err(InvalidTritValueError(2)));
        assert_eq!(Trit::try_from(-2), Err(InvalidTritValueError(-2)));
        assert_eq!(Trit::try_from(i8::MAX), Err(InvalidTritValueError(i8::MAX)));
        assert_eq!(Trit::try_from(i8::MIN), Err(InvalidTritValueError(i8::MIN)));
    }
}