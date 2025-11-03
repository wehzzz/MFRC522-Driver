use kernel::prelude::CStr;

pub const DEVICE_NAME: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"mfrc522\0") };
pub const MFRC522_BUFSIZE: usize = 25;

pub const MFRC522_CMDREG: u8 = 0x01;
pub const MFRC522_CMDREG_CMD: u8 = 0x0F;
pub const MFRC522_CMDREG_SOFTPWRDOWN: u8 = 0x10;
pub const MFRC522_CMDREG_RCVOFF: u8 = 0x20;
pub const MFRC522_FIFODATAREG: u8 = 0x09;
pub const MFRC522_FIFOLEVELREG: u8 = 0x0A;
pub const MFRC522_FIFOLEVELREG_LEVEL: u8 = 0x7F;
pub const MFRC522_FIFOLEVELREG_FLUSH: u8 = 0x80;

pub const MFRC522_IDLE: u8 = 0x0;
pub const MFRC522_MEM: u8 = 0x1;
pub const MFRC522_GENERATERANDOMID: u8 = 0x2;
pub const MFRC522_NOCMDCHANGE: u8 = 0x7;
pub const MFRC522_SOFTRESET: u8 = 0xF;

#[repr(C)]
pub struct Buffer {
    pub buffer: [u8; MFRC522_BUFSIZE],
    pub to_read: usize,
}

impl Default for Buffer {
    fn default() -> Self {
        Self {
            buffer: [0; MFRC522_BUFSIZE],
            to_read: 0,
        }
    }
}
