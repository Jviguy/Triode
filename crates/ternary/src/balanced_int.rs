use crate::trit::Trit;


pub trait TernaryIntegerRepr {
    /// The smallest integer type that can be used for balanced ternary representation of N trits.
    /// This has to be a signed integer type and big enough to hold 3^N - 1 / 2.
    /// For example, for 6 trits (a tryte), we need at least i16 because 3^6 - 1 / 2 = 364.
    /// For 12 trits, we need at least i32 because 3^12 - 1 / 2 = 531440.
    /// For 20 trits, we need at least i64 because 3^20 - 1 / 2 = 17433922025.
    type Int; // The primary integer type (i16, i64, etc.)
}

pub trait ArithmeticTernaryInteger {}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct BalancedInt<const N: usize>([Trit; N]);

impl<const N: usize> BalancedInt<N> where Self: TernaryIntegerRepr {
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
}

impl<const N: usize> Default for BalancedInt<N> where Self: TernaryIntegerRepr {
    fn default() -> Self {
        Self::zero()
    }
}

impl<const N: usize> From<Trit> for BalancedInt<N> where Self: TernaryIntegerRepr {
    fn from(trit: Trit) -> Self {
        let mut trits = [Trit::Zero; N];
        trits[0] = trit;
        BalancedInt(trits)
    }
}

impl<const N: usize> From<[Trit; N]> for BalancedInt<N> where Self: TernaryIntegerRepr {
    fn from(trits: [Trit; N]) -> Self {
        BalancedInt(trits)
    }
}

impl<const N: usize> std::fmt::Display for BalancedInt<N> where Self: TernaryIntegerRepr {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let trit_strs: Vec<String> = self.0.iter().map(|t| format!("{:?}", t)).collect();
        write!(f, "BalancedInt[{}]", trit_strs.join(", "))
    }
}

impl<const N: usize> std::ops::Index<usize> for BalancedInt<N> where Self: TernaryIntegerRepr {
    type Output = Trit;

    fn index(&self, index: usize) -> &Self::Output {
        &self.0[index]
    }
}

impl<const N: usize> std::ops::IndexMut<usize> for BalancedInt<N> where Self: TernaryIntegerRepr {
    fn index_mut(&mut self, index: usize) -> &mut Self::Output {
        &mut self.0[index]
    }
}

// Arthimetic operations.

impl<const N: usize> BalancedInt<N> where Self: TernaryIntegerRepr + ArithmeticTernaryInteger {
    pub fn full_add(&self, other: &Self, carry_in: Trit) -> (Self, Trit) {
        let mut result = [Trit::Zero; N];
        let mut carry = carry_in;

        for i in 0..N {
            let (sum, new_carry) = self.0[i].full_add(other.0[i], carry);
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
}