mod parse;

use derive_try_from_primitive::TryFromPrimitive;

#[derive(Debug)]
pub struct File {
    pub class: Class,

    pub r#type: Type,
    pub machine: Machine,
    pub entry_point: Addr,
}

impl File {
    const MAGIC: &'static [u8] = &[0x7f, 0x45, 0x4c, 0x46];

    pub fn parse(i: parse::Input) -> parse::Result<Self> {
        use nom::{
            bytes::complete::{tag, take},
            combinator::verify,
            error::context,
            number::complete::le_u32,
            sequence::tuple,
        };
        let (i, (_, class, ..)) = tuple((
            // ----------
            context("Magic", tag(Self::MAGIC)),
            Class::parse,
            context("Endianness", tag(&[0x1])),
            context("Version", tag(&[0x1])),
            context("OS ABI", nom::branch::alt((tag(&[0x0]), tag(&[0x3])))),
            // ----------
            context("Padding", take(8_usize)),
        ))(i)?;

        let (i, (r#type, machine)) = tuple((Type::parse, Machine::parse))(i)?;
        // this 32-bit integer should always be set to 1 in the current
        // version of ELF, see the diagram. We don't *have* to check it,
        // but it's so easy to, let's anyway!
        let (i, _) = context("Version (bis)", verify(le_u32, |&x| x == 1))(i)?;
        let (i, entry_point) = Addr::parse(i)?;

        let res = Self {
            class,
            r#type,
            machine,
            entry_point,
        };

        Ok((i, res))
    }

    pub fn parse_or_print_error(i: parse::Input) -> Option<Self> {
        match Self::parse(i) {
            Ok((_, file)) => Some(file),
            Err(nom::Err::Error(e)) | Err(nom::Err::Failure(e)) => {
                eprintln!("Parsing failed:");
                for (input, err) in e.errors {
                    use nom::Offset;
                    let offset = i.offset(input);
                    eprintln!("{:?} at position {}:", err, offset);
                    eprintln!("{:>08x}: {:?}", offset, HexDump(input))
                }
                None
            }
            Err(_) => panic!("unexpected nom error"),
        }
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, TryFromPrimitive)]
#[repr(u8)]
pub enum Class {
    // TODO(Niz): support i386
    // I386 = 0x01,
    X86_64 = 0x02,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, TryFromPrimitive)]
#[repr(u16)]
pub enum Type {
    None = 0x0,
    Rel = 0x1,
    Exec = 0x2,
    Dyn = 0x3,
    Core = 0x4,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, TryFromPrimitive)]
#[repr(u16)]
pub enum Machine {
    X86 = 0x3,
    X86_64 = 0x3e,
}

impl_parse_for_enum!(Class, le_u8);
impl_parse_for_enum!(Type, le_u16);
impl_parse_for_enum!(Machine, le_u16);

#[cfg(test)]
mod tests {
    use super::Machine;
    use super::Type;

    #[test]
    fn type_to_u16() {
        assert_eq!(Type::Dyn as u16, 0x3);
    }
    #[test]
    fn type_from_u16() {
        assert_eq!(Type::try_from(0x3), Ok(Type::Dyn));
        assert_eq!(Type::try_from(0xf00d), Err(0xf00d));
    }

    #[test]
    fn try_enums() {
        assert_eq!(Machine::X86 as u16, 0x3);
        assert_eq!(Machine::try_from(0x3E), Ok(Machine::X86_64));
        assert_eq!(Machine::try_from(0xFA), Err(0xFA));
    }
}

pub struct HexDump<'a>(&'a [u8]);
use std::fmt;

impl<'a> fmt::Debug for HexDump<'a> {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        for &x in self.0.iter().take(20) {
            write!(f, "{:02x} ", x)?;
        }
        Ok(())
    }
}

use derive_more::Add;
use derive_more::Sub;

#[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Add, Sub)]
pub struct Addr(pub u64);

impl fmt::Debug for Addr {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:08x}", self.0)
    }
}

impl fmt::Display for Addr {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        fmt::Debug::fmt(&self, f)
    }
}

// This will come in handy when serializing
impl Into<u64> for Addr {
    fn into(self) -> u64 {
        self.0
    }
}

// This will come in handy when indexing / sub-slicing slices
impl Into<usize> for Addr {
    fn into(self) -> usize {
        self.0 as usize
    }
}

impl Addr {
    pub fn parse(i: parse::Input) -> parse::Result<Self> {
        use nom::{combinator::map, number::complete::le_u64};
        map(le_u64, Addr)(i)
    }
}
