use kernel::prelude::CStr;

pub const DEVICE_NAME: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"mfrc522\0") };
pub const MFRC522_BUFSIZE: usize = 25;

#[derive(Debug)]
#[repr(u8)]
pub enum Register {
    Mfrc522CmdReg = 0x01,
    Mfrc522FifoDataReg = 0x09,
    Mfrc522FifoLevelReg = 0x0A,
    Mfrc522FifoLevelRegLevel = 0x7F,
    Mfrc522FifoLevelRegFlush = 0x80,
}

pub enum Cmd {
    Mfrc522Idle = 0x0,
    Mfrc522Mem = 0x1,
    Mfrc522GenerateRandomId = 0x2,
    Mfrc522NoCmdChange = 0x7,
    Mfrc522SoftReset = 0xF,
}

impl Cmd {
    pub fn to_payload(self) -> [u8; 1] {
        return [self as u8];
    }
}

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
