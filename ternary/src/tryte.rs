use crate::trit::Trit;
use crate::utils::pow3;

const TRITS_PER_TRYTE: usize = 6;
const TRYTE_MAX: i64 = (pow3(TRITS_PER_TRYTE) - 1)/ 2;
const TRYTE_MIN: i64 = -TRYTE_MAX;
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct Tryte([Trit; TRITS_PER_TRYTE]);

impl Tryte {
    pub fn zero() -> Self {
        Tryte([Trit::Zero; TRITS_PER_TRYTE])
    }

    pub fn iter(&self) -> std::slice::Iter<'_, Trit> {
        self.0.iter()
    }
}

impl Default for Tryte {
    fn default() -> Self {
        Self::zero()
    }
}

impl From<Trit> for Tryte {
    fn from(trit: Trit) -> Self {
        let mut trits = [Trit::Zero; TRITS_PER_TRYTE];
        trits[0] = trit;
        Tryte(trits)
    }
}

impl From<[Trit; TRITS_PER_TRYTE]> for Tryte {
    fn from(trits: [Trit; TRITS_PER_TRYTE]) -> Self {
        Tryte(trits)
    }
}

impl std::fmt::Display for Tryte {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let trit_strs: Vec<String> = self.0.iter().map(|t| format!("{:?}", t)).collect();
        write!(f, "Tryte({})", trit_strs.join(", "))
    }
}

impl std::ops::Index<usize> for Tryte {
    type Output = Trit;

    fn index(&self, index: usize) -> &Self::Output {
        &self.0[index]
    }
}

impl std::ops::IndexMut<usize> for Tryte {
    fn index_mut(&mut self, index: usize) -> &mut Self::Output {
        &mut self.0[index]
    }
}

// From int conversions:
// Guaranteed to be infallible, so we use From

impl From<i8> for Tryte { fn from(value: i8) -> Self { Tryte::try_from(value as i16).unwrap() } }
impl From<u8> for Tryte { fn from(value: u8) -> Self { Tryte::try_from(value as i16).unwrap() } }


#[derive(Debug, PartialEq, Eq)]
pub enum TryteConversionError {
    ValueOutOfRange(i16),
    ValueOutOfRangeUnsigned(u16),
    NegativeValueNotAllowed(i16),
}

impl std::fmt::Display for TryteConversionError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::ValueOutOfRange(v) => write!(f, "Value {} is out of range for Tryte ({} to {})", v, TRYTE_MIN, TRYTE_MAX),
            Self::ValueOutOfRangeUnsigned(v) => write!(f, "Unsigned value {} is out of range for Tryte (0 to {})", v, TRYTE_MAX),
            Self::NegativeValueNotAllowed(v) => write!(f, "Negative value {} cannot be represented as an unsigned integer", v),
        }
    }
}

impl std::error::Error for TryteConversionError {}


// Not guaranteed to be infallible, so we use TryFrom

impl TryFrom<i16> for Tryte {
    type Error = TryteConversionError;

    fn try_from(value: i16) -> Result<Self, Self::Error> {
        if value < TRYTE_MIN as i16 || value > TRYTE_MAX as i16 {
            return Err(TryteConversionError::ValueOutOfRange(value));
        }

        let mut trits = [Trit::Zero; TRITS_PER_TRYTE];
        let mut remainder = value;

        for i in 0..TRITS_PER_TRYTE {
            let trit_value: i8 = (((remainder + 1).rem_euclid(3)) - 1) as i8;
            // Should be fine to unwrap lol.
            // might need to check on this so TODO.
            trits[i] = Trit::try_from(trit_value).unwrap();
            remainder = (remainder - trit_value as i16) / 3;
        }

        Ok(Tryte(trits))
    }
}

impl TryFrom<u16> for Tryte {
    type Error = TryteConversionError;

    fn try_from(value: u16) -> Result<Self, Self::Error> {
        if value > TRYTE_MAX as u16 { return Err(TryteConversionError::ValueOutOfRangeUnsigned(value)); }
        Ok(Tryte::try_from(value as i16)?)
    }
}

// To int conversion:

impl From<Tryte> for i16 {
    fn from(tr: Tryte) -> Self {
        let mut value: i16 = 0;
        let mut power: i16 = 1;
        for (_i, trit) in tr.iter().enumerate() {
            value += (*trit as i16) * power;
            power *= 3;
        }
        value
    }
}
