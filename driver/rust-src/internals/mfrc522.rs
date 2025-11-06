use kernel::prelude::CStr;

pub(crate) const DEVICE_NAME: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"mfrc522\0") };
pub(crate) const MFRC522_BUFSIZE: usize = 25;

pub(crate) enum Register {
    Mfrc522CmdReg = 0x01,
    Mfrc522FifoDataReg = 0x09,
    Mfrc522FifoLevelReg = 0x0A,
    Mfrc522FifoLevelRegFlush = 0x80,
}

pub(crate) enum Cmd {
    _Mfrc522Idle = 0x0,
    Mfrc522Mem = 0x1,
    Mfrc522GenerateRandomId = 0x2,
    _Mfrc522NoCmdChange = 0x7,
    _Mfrc522SoftReset = 0xF,
}

pub(crate) struct Buffer {
    pub(crate) buffer: [u8; MFRC522_BUFSIZE],
    pub(crate) to_read: usize,
}

impl Default for Buffer {
    fn default() -> Self {
        Self {
            buffer: [0; MFRC522_BUFSIZE],
            to_read: 0,
        }
    }
}
