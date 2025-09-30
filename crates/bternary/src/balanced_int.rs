use std::cmp::{Ordering, PartialOrd};
use std::ops::Range;
use thiserror::Error;
use crate::balanced_int::BIntError::RangeInvalid;
use crate::trit::Trit;

pub trait Int:
Copy
+ From<i8>
+ std::ops::Neg<Output = Self>
+ std::ops::Add<Output = Self>
+ std::ops::Sub<Output = Self>
+ std::ops::Mul<Output = Self>
+ std::ops::Div<Output = Self>
+ std::ops::Rem<Output = Self>
+ std::ops::AddAssign
+ std::ops::SubAssign
+ std::ops::MulAssign
+ std::ops::DivAssign
+ std::ops::RemAssign
+ TryFrom<i64>
+ Ord
{
    fn zero() -> Self;
    fn one() -> Self;
    fn two() -> Self;
    fn three() -> Self;
}
impl Int for i16 {
    fn zero() -> Self { 0 }
    fn one() -> Self { 1 }
    fn two() -> Self { 2 }
    fn three() -> Self { 3 }
}
impl Int for i64 {
    fn zero() -> Self { 0 }
    fn one() -> Self { 1 }
    fn two() -> Self { 2 }
    fn three() -> Self { 3 }
}

pub trait TernaryIntegerRepr {
    /// The smallest integer type that can be used for balanced bternary representation of N trits.
    /// This has to be a signed integer type and big enough to hold 3^N - 1 / 2.
    /// For example, for 6 trits (a tryte), we need at least i16 because 3^6 - 1 / 2 = 364.
    /// For 12 trits, we need at least i32 because 3^12 - 1 / 2 = 531440.
    /// For 20 trits, we need at least i64 because 3^20 - 1 / 2 = 17433922025.
    type Int: Int;
}

pub trait ArithmeticTernaryInteger {}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct BalancedInt<const N: usize>([Trit; N]);

impl<const N: usize> BalancedInt<N> {
    pub fn zero() -> Self {
        BalancedInt([Trit::Zero; N])
    }

    pub fn new(trits: [Trit; N]) -> Self {
        BalancedInt(trits)
    }

    pub fn iter(&self) -> std::slice::Iter<'_, Trit> {
        self.0.iter()
    }

    pub fn iter_mut(&mut self) -> std::slice::IterMut<'_, Trit> {
        self.0.iter_mut()
    }

    pub fn is_zero(&self) -> bool {
        self.iter().all(|&t| t == Trit::Zero)
    }

    pub fn sign(&self) -> Trit {
        for &t in self.iter().rev() {
            if t != Trit::Zero {
                return t;
            }
        }
        Trit::Zero
    }
}

impl<const N: usize> Default for BalancedInt<N> {
    fn default() -> Self {
        Self::zero()
    }
}

impl<const N: usize> From<Trit> for BalancedInt<N> {
    fn from(trit: Trit) -> Self {
        let mut trits = [Trit::Zero; N];
        trits[0] = trit;
        BalancedInt(trits)
    }
}

impl<const N: usize> From<[Trit; N]> for BalancedInt<N> {
    fn from(trits: [Trit; N]) -> Self {
        BalancedInt(trits)
    }
}

impl<const N: usize> std::fmt::Display for BalancedInt<N> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let trit_strs: Vec<String> = self.iter().rev().map(|t| format!("{}", t)).collect();
        write!(f, "{}", trit_strs.join(""))
    }
}

impl<const N: usize> std::ops::Index<usize> for BalancedInt<N> {
    type Output = Trit;

    fn index(&self, index: usize) -> &Self::Output {
        &self.0[index]
    }
}

impl<const N: usize> std::ops::IndexMut<usize> for BalancedInt<N> {
    fn index_mut(&mut self, index: usize) -> &mut Self::Output {
        &mut self.0[index]
    }
}

impl<const N: usize> PartialOrd<Self> for BalancedInt<N> {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

impl<const N: usize> Ord for BalancedInt<N> {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        // just look for most sig trit and deal with signs
        for i in (0..N).rev() {
            if self[i] > other[i] {
                return Ordering::Greater;
            } else if self[i] < other[i] {
                return Ordering::Less;
            }
        }
        Ordering::Equal
    }
}

impl<const N: usize> BalancedInt<N>
where
    Self: TernaryIntegerRepr,
    <Self as TernaryIntegerRepr>::Int: Int,
{
    pub fn to_int(self) -> <Self as TernaryIntegerRepr>::Int {
        let mut acc = <Self as TernaryIntegerRepr>::Int::zero();
        for i in (0..N).rev() {
            acc *= <Self as TernaryIntegerRepr>::Int::three();
            acc += <Self as TernaryIntegerRepr>::Int::from(self[i] as i8);
        }
        acc
    }

    pub fn from_int(mut value: <Self as TernaryIntegerRepr>::Int) -> Self {
        let mut trits = [Trit::Zero; N];

        let zero = <Self as TernaryIntegerRepr>::Int::zero();
        let one = <Self as TernaryIntegerRepr>::Int::one();
        let two = <Self as TernaryIntegerRepr>::Int::two();
        let three = <Self as TernaryIntegerRepr>::Int::three();

        for i in 0..N {
            if value == zero {
                break; // The number is fully converted.
            }

            let mut rem = value % three;
            value /= three;

            // Balance the remainder to be in the set {-1, 0, 1}
            if rem == two {
                rem = <Self as TernaryIntegerRepr>::Int::from(-1);
                value += one;
            } else if rem == <Self as TernaryIntegerRepr>::Int::from(-2) { // This was the missing case
                rem = one;
                value -= one;
            }

            trits[i] = match rem {
                r if r == <Self as TernaryIntegerRepr>::Int::from(-1) => Trit::Neg,
                r if r == zero => Trit::Zero,
                r if r == one => Trit::Pos,
                _ => unreachable!("Remainder balancing failed. This is a bug."),
            };
        }
        BalancedInt(trits)
    }

    /// reads a range of trits.
    /// TODO: Change this to use a slice method that is basically a reference span of start to end makes this better as its still doing TryFrom<i64> which means there is a limit.
    pub fn read_trit_range(&self, start: usize, end: usize) -> Result<<Self as TernaryIntegerRepr>::Int, BIntError>
    {
        if start > end || end >= N {
            Err(RangeInvalid(start, end))
        } else {
            let mut value = 0i64;
            for i in (start..=end).rev() {
                value *= 3;
                value += self[i] as i64;
            }
            // Safely try to convert the i64 result into the requested type `T`
            <Self as TernaryIntegerRepr>::Int::try_from(value).map_err(BIntError::ValueRange)
        }
    }

    pub fn write_trit_range<T>(&mut self, value: T, start: usize, end: usize) -> Result<<Self as TernaryIntegerRepr>::Int, BIntError>
    where
        T: Copy + TryInto<i64>,
    {
        // --- Input Validation ---
        assert!(start <= end, "Start of range cannot be after the end.");
        assert!(end < N, "End of range is out of bounds for this Word size.");

        let mut num = match value.try_into() {
            Ok(n) => n,
            Err(_) => return Err(),
        };

        // --- Conversion and Writing Loop ---
        // Iterate from the least significant trit (start) to the most significant (end).
        for i in start..=end {
            let remainder = (num + 1) % 3 - 1;

            self[i] = Trit::try_from(remainder as i8).unwrap();

            num = (num - remainder) / 3;
        }

        if num != 0 {
            return Err("Value out of range for the given number of trits.");
        }

        Ok(())
    }}
}

#[derive(Error, Debug)]
pub enum BIntError {
    #[error("Invalid range {0}->{1}")]
    RangeInvalid(usize, usize),
    #[error("Cannot fit value in the trit range into the type requested.")]
    ValueRange(#[from] std::num::TryFromIntError),
}

impl<const N: usize> BalancedInt<N> {
    /// Reads a range of trits and tries to convert them into a requested integer type `T`.
    /// TODO: maybe just fix this entirely as it feels very weird.
}
// Arthimetic operations.

impl<const N: usize> BalancedInt<N> where Self: ArithmeticTernaryInteger {
    pub fn full_add(&self, other: &Self, carry_in: Trit) -> (Self, Trit) {
        let mut result = [Trit::Zero; N];
        let mut carry = carry_in;

        for i in 0..N {
            let (sum, new_carry) = self[i].full_add(other[i], carry);
            result[i] = sum;
            carry = new_carry;
        }

        (BalancedInt(result), carry)
    }

    pub fn negate(&self) -> Self {
        let mut result = [Trit::Zero; N];
        for i in 0..N {
            result[i] = self.0[i].negate();
        }
        BalancedInt(result)
    }

    /// Left trit shift (multiply by 3)
    /// Returns a new BalancedInt with all trits shifted left by one position.
    pub fn shift_left(&self, amt: usize) -> Self {
        let mut result = [Trit::Zero; N];
        for i in amt..N {
            result[i] = self[i - amt];
        }
        BalancedInt(result)
    }


    pub fn abs(&self) -> Self {
        if self.sign() == Trit::Neg {
            self.negate()
        } else {
            // copy that jawn its N bytes lol.
            *self
        }
    }

    /// Division with remainder.
    /// Returns (quotient, remainder)
    /// Panics if rhs is zero.
    /// Uses a simple long-division algorithm.
    /// This is not optimized for performance.
    /// TODO: holy optimize, change it idk I just want something works for now.
    fn div_rem(self, rhs: Self) -> (Self, Self) {
        if rhs.is_zero() {
            panic!("Division by zero");
        }

        let mut remainder = self;
        let mut quotient = Self::zero();

        // Use the sign of rhs to correct the quotient later.
        // Work with a positive divisor to simplify the logic.
        let divisor_sign = rhs.sign();
        let divisor = rhs.abs();
        let mut divisor_msb_pos = 0;
        for i in (0..N).rev() {
            if divisor[i] != Trit::Zero {
                divisor_msb_pos = i;
                break;
            }
        }

        for i in (0..(N - divisor_msb_pos)).rev() {
            let shifted_divisor = divisor.shift_left(i);

            // Try subtracting the shifted divisor. If this makes the remainder's absolute value
            // smaller, then the quotient trit is likely 1.
            let rem_after_sub = remainder - shifted_divisor;
            if rem_after_sub.abs() <= remainder.abs() {
                remainder = rem_after_sub;
                quotient[i] = Trit::Pos;
                continue; // Move to the next lower trit position
            }

            // Try adding the shifted divisor. If this makes the remainder's absolute value
            // smaller, then the quotient trit is likely -1.
            let rem_after_add = remainder + shifted_divisor;
            if rem_after_add.abs() <= remainder.abs() {
                remainder = rem_after_add;
                quotient[i] = Trit::Neg;
            }

            // If neither operation reduced the remainder's magnitude, the quotient trit is 0.
        }

        // Correct the quotient's sign based on the original divisor's sign
        if divisor_sign == Trit::Neg {
            quotient = quotient.negate();
        }

        (quotient, remainder)
    }
}

impl<const N: usize> std::ops::Neg for BalancedInt<N> where Self: ArithmeticTernaryInteger {
    type Output = Self;

    fn neg(self) -> Self::Output {
        self.negate()
    }
}

impl<const N: usize> std::ops::Add for BalancedInt<N> where Self: ArithmeticTernaryInteger {
    type Output = Self;

    fn add(self, rhs: Self) -> Self::Output {
        let (sum, _carry) = self.full_add(&rhs, Trit::Zero);
        sum
    }
}

impl<const N: usize> std::ops::AddAssign for BalancedInt<N> where Self: ArithmeticTernaryInteger {
    fn add_assign(&mut self, rhs: Self) {
        let mut carry = Trit::Zero;
        for i in 0..N {
            let (sum, new_carry) = self[i].full_add(rhs[i], carry);
            self[i] = sum;
            carry = new_carry;
        }
    }
}

impl<const N: usize> std::ops::Sub for BalancedInt<N> where Self: ArithmeticTernaryInteger {
    type Output = Self;

    fn sub(self, rhs: Self) -> Self::Output {
        self + rhs.negate()
    }
}

impl<const N: usize> std::ops::SubAssign for BalancedInt<N> where Self: ArithmeticTernaryInteger {
    fn sub_assign(&mut self, rhs: Self) {
        *self += rhs.negate();
    }
}

impl<const N: usize> std::ops::MulAssign for BalancedInt<N> where Self: ArithmeticTernaryInteger {
    fn mul_assign(&mut self, rhs: Self) {
        // copy of self to use as multiplicand.
        let multiplicand = *self;

        let mut accumulator = Self::zero();

        for i in 0..N {
            let multiplier_trit = rhs[i];

            let partial_product = match multiplier_trit {
                Trit::Pos => multiplicand,
                Trit::Zero => continue,
                Trit::Neg => multiplicand.negate(),
            };

            let shifted_product = partial_product.shift_left(i);

            accumulator += shifted_product;
        }

        *self = accumulator;
    }
}

impl<const N: usize> std::ops::Mul for BalancedInt<N> where Self: ArithmeticTernaryInteger {
    type Output = Self;

    fn mul(mut self, rhs: Self) -> Self::Output {
        self *= rhs;
        self
    }
}

impl<const N: usize> std::ops::Div for BalancedInt<N> where Self: ArithmeticTernaryInteger {
    type Output = Self;

    fn div(self, rhs: Self) -> Self::Output {
        let (quotient, _remainder) = self.div_rem(rhs);
        quotient
    }
}

impl<const N: usize> std::ops::DivAssign for BalancedInt<N> where Self: ArithmeticTernaryInteger {
    fn div_assign(&mut self, rhs: Self) {
        let (quotient, _remainder) = self.div_rem(rhs);
        *self = quotient;
    }
}

impl<const N: usize> std::ops::Rem for BalancedInt<N> where Self: ArithmeticTernaryInteger {
    type Output = Self;

    fn rem(self, rhs: Self) -> Self::Output {
        let (_quotient, remainder) = self.div_rem(rhs);
        remainder
    }
}

impl<const N: usize> std::ops::RemAssign for BalancedInt<N> where Self: ArithmeticTernaryInteger {
    fn rem_assign(&mut self, rhs: Self) {
        let (_quotient, remainder) = self.div_rem(rhs);
        *self = remainder;
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::Word;

    // Helper to quickly convert an i16 to a Tryte for testing.
    fn from_i64(val: i64) -> Word {
        Word::from_int(val)
    }

    #[test]
    fn test_zero_conversion() {
        let zero_int = 0;
        let zero_tryte = from_i64(zero_int);
        assert!(zero_tryte.is_zero());
        assert_eq!(zero_tryte.to_int(), zero_int);
    }

    #[test]
    fn test_positive_conversions() {
        let values: &[(i64, &str)] = &[
            (1, "000000000000000000000001"),
            (3, "000000000000000000000010"),
            (10, "000000000000000000000101"),
            (13, "000000000000000000000111"),
            (364, "000000000000000000111111"), // Max value for 6 trits
        ];

        for &(val, s) in values {
            let tryte = from_i64(val);
            assert_eq!(tryte.to_string(), s, "Display for {}", val);
            assert_eq!(tryte.to_int(), val, "to_int for {}", val);
        }
    }

    #[test]
    fn test_negative_conversions() {
        let values: &[(i64, &str)] = &[
            (-1, "00000000000000000000000T"),
            (-3, "0000000000000000000000T0"),
            (-10, "000000000000000000000T0T"),
            (-13, "000000000000000000000TTT"),
            (-364, "000000000000000000TTTTTT"), // Min value for 6 trits
        ];

        for &(val, s) in values {
            let tryte = from_i64(val);
            assert_eq!(tryte.to_string(), s, "Display for {}", val);
            assert_eq!(tryte.to_int(), val, "to_int for {}", val);
        }
    }

    #[test]
    fn test_round_trip_conversions() {
        // Max value for 6 trits is (3^6 - 1) / 2 = 364
        for i in -364..=364 {
            let tryte = from_i64(i);
            assert_eq!(tryte.to_int(), i, "Round trip failed for {}", i);
        }
    }

    #[test]
    fn test_sign_method() {
        assert_eq!(from_i64(0).sign(), Trit::Zero);
        assert_eq!(from_i64(100).sign(), Trit::Pos);
        assert_eq!(from_i64(-100).sign(), Trit::Neg);
        // Test a number where the MSD is 0 but it's still positive
        let thirteen = from_i64(13); // 000111
        assert_eq!(thirteen[5], Trit::Zero);
        assert_eq!(thirteen.sign(), Trit::Pos);
    }

    #[test]
    fn test_negation() {
        assert_eq!(from_i64(0).negate(), from_i64(0));
        assert_eq!((-from_i64(123)).to_int(), -123);
        assert_eq!((-from_i64(-55)).to_int(), 55);
        // Test double negation
        let val = from_i64(99);
        assert_eq!(-(-val), val);
    }

    #[test]
    fn test_addition() {
        assert_eq!((from_i64(5) + from_i64(3)).to_int(), 8);
        assert_eq!((from_i64(-5) + from_i64(3)).to_int(), -2);
        assert_eq!((from_i64(5) + from_i64(-3)).to_int(), 2);
        assert_eq!((from_i64(-5) + from_i64(-3)).to_int(), -8);
        assert_eq!((from_i64(10) + from_i64(0)).to_int(), 10);
        // Test overflow
        let max = from_i64(141_214_768_240);
        let one = from_i64(1);
        assert_eq!((max + one).to_int(), -141_214_768_240); // Wraps around
    }

    #[test]
    fn test_subtraction() {
        assert_eq!((from_i64(5) - from_i64(3)).to_int(), 2);
        assert_eq!((from_i64(-5) - from_i64(3)).to_int(), -8);
        assert_eq!((from_i64(5) - from_i64(-3)).to_int(), 8);
        assert_eq!((from_i64(-5) - from_i64(-3)).to_int(), -2);
        assert_eq!((from_i64(10) - from_i64(0)).to_int(), 10);
    }

    #[test]
    fn test_multiplication() {
        assert_eq!((from_i64(5) * from_i64(3)).to_int(), 15);
        assert_eq!((from_i64(-5) * from_i64(3)).to_int(), -15);
        assert_eq!((from_i64(5) * from_i64(-3)).to_int(), -15);
        assert_eq!((from_i64(-5) * from_i64(-3)).to_int(), 15);
        assert_eq!((from_i64(10) * from_i64(0)).to_int(), 0);
        assert_eq!((from_i64(10) * from_i64(1)).to_int(), 10);
        assert_eq!((from_i64(10) * from_i64(-1)).to_int(), -10);
    }

    #[test]
    fn test_division() {
        assert_eq!((from_i64(10) / from_i64(3)).to_int(), 3);
        assert_eq!((from_i64(10) / from_i64(-3)).to_int(), -3);
        assert_eq!((from_i64(-10) / from_i64(3)).to_int(), -3);
        assert_eq!((from_i64(-10) / from_i64(-3)).to_int(), 3);
        assert_eq!((from_i64(0) / from_i64(5)).to_int(), 0);
        assert_eq!((from_i64(3) / from_i64(10)).to_int(), 0);
        assert_eq!((from_i64(364) / from_i64(10)).to_int(), 36);
    }

    #[test]
    #[should_panic(expected = "Division by zero")]
    fn test_division_by_zero_panics() {
        let _ = from_i64(10) / from_i64(0);
    }

    #[test]
    fn test_remainder() {
        // Balanced ternary remainder is always between -rhs/2 and +rhs/2
        assert_eq!((from_i64(10) % from_i64(3)).to_int(), 1);
        assert_eq!((from_i64(10) % from_i64(-3)).to_int(), 1);
        assert_eq!((from_i64(-10) % from_i64(3)).to_int(), -1);
        assert_eq!((from_i64(-10) % from_i64(-3)).to_int(), -1);
        assert_eq!((from_i64(5) % from_i64(3)).to_int(), -1); // 5 = 2*3 - 1
        assert_eq!((from_i64(0) % from_i64(3)).to_int(), 0);
    }

    #[test]
    fn test_shift_left() {
        // Shifting left by 1 is multiplication by 3
        assert_eq!(from_i64(10).shift_left(1).to_int(), 30);
        assert_eq!(from_i64(-15).shift_left(1).to_int(), -45);
        assert_eq!(from_i64(15_690_529_805).shift_left(2).to_int(), -141_214_768_236); // (MAX/9 + 1) * 9        assert_eq!(from_i64(100).shift_left(0).to_int(), 100);
        // Shift all trits out
        assert!(from_i64(300).shift_left(24).is_zero());
        assert!(from_i64(300).shift_left(30).is_zero());
    }

    #[test]
    fn test_display_format() {
        // String format should have no spaces or extra chars, just trits
        assert_eq!(from_i64(13).to_string(), "000000000000000000000111");
        assert_eq!(from_i64(-13).to_string(), "000000000000000000000TTT");
    }
}
