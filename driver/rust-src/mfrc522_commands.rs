use core::i32;
use core::str::FromStr;

use kernel::bindings;
use kernel::error::{Error, Result};
use kernel::prelude::{EINVAL, ENODEV};
use kernel::{pr_err, pr_info};

use crate::internals::{
    mfrc522::{Cmd::*, Register::*, MFRC522_BUFSIZE},
    spi::Spi,
};
use crate::mfrc522_spi::{spi_read, spi_write};
use crate::Mfrc522Device;
use crate::G_MFRC522;

const __LOG_PREFIX: &[u8] = b"mfrc522";

const CMDARGS_SEP: &str = ":";
const MEM_WRITE: &str = "mem_write";
const MEM_READ: &str = "mem_read";
const GENERATE_RANDOM_ID: &str = "gen_rand_id";

type Command = fn(&str) -> Result<i32, Error>;

fn command_dispatch(cmd: &str) -> Result<Command, Error> {
    match cmd {
        MEM_WRITE => Ok(mem_write),
        MEM_READ => Ok(mem_read),
        GENERATE_RANDOM_ID => Ok(generate_random_id),
        _ => {
            pr_err!("MFRC522: Parse command: Unknown command: {}", cmd);
            Err(EINVAL)
        }
    }
}

pub fn command_handle(cmd: &str) -> Result<i32, Error> {
    let (cmd_name, args): (&str, &str) = match cmd.split_once(CMDARGS_SEP) {
        None => (cmd, ""),
        Some(arg) => arg,
    };
    command_dispatch(cmd_name)?(args)
}

fn reset_internal_memory() -> Result<(), Error> {
    let mfrc522 = unsafe {
        match &mut G_MFRC522 {
            Some(mfrc) => mfrc,
            None => return Err(ENODEV),
        }
    };

    for _ in 0..(MFRC522_BUFSIZE) {
        spi_write(
            &mut mfrc522.spi,
            &(Mfrc522FifoDataReg as u8),
            &mut (0 as u8),
        )?;
    }

    spi_write(
        &mut mfrc522.spi,
        &(Mfrc522CmdReg as u8),
        &mut (Mfrc522Mem as u8),
    )?;
    Ok(())
}

fn mem_write(args: &str) -> Result<i32, Error> {
    let mfrc522 = unsafe {
        match &mut G_MFRC522 {
            Some(mfrc) => mfrc,
            None => return Err(ENODEV),
        }
    };

    let (len, data): (i32, &str) = match args.split_once(CMDARGS_SEP) {
        Some((len_, data_)) => (
            (len_)
                .parse::<i32>()
                .map_err(|_| {
                    pr_err!("MFRC522: Parse command: failed to convert length to unsigned\n");
                    EINVAL
                })
                .map(|len| core::cmp::min(len, MFRC522_BUFSIZE as i32))?,
            data_,
        ),
        None => {
            return {
                pr_err!("MFRC522: Parse command: failed to extract length\n");
                Err(EINVAL)
            }
        }
    };

    for (i, c) in data.char_indices() {
        if (i as i32) < len {
            spi_write(
                &mut mfrc522.spi,
                &(Mfrc522FifoDataReg as u8),
                &mut (c as u8),
            )?;
        }
    }
    for _ in 0..(MFRC522_BUFSIZE as i32 - len) {
        spi_write(
            &mut mfrc522.spi,
            &(Mfrc522FifoDataReg as u8),
            &mut (0 as u8),
        )?;
    }

    spi_write(
        &mut mfrc522.spi,
        &(Mfrc522CmdReg as u8),
        &mut (Mfrc522Mem as u8),
    )?;
    Ok(MFRC522_BUFSIZE as i32)
}

fn mem_read(args: &str) -> Result<i32, Error> {
    let mfrc522 = unsafe {
        match G_MFRC522.as_mut() {
            Some(dev) => &mut **dev,
            None => return Err(ENODEV),
        }
    };

    spi_write(
        &mut mfrc522.spi,
        &(Mfrc522FifoLevelReg as u8),
        &mut (Mfrc522FifoLevelRegFlush as u8),
    )?;

    spi_write(
        &mut mfrc522.spi,
        &(Mfrc522CmdReg as u8),
        &mut (Mfrc522Mem as u8),
    )?;

    for chr in &mut mfrc522.buffer.buffer.iter_mut() {
        *chr = spi_read(&mut mfrc522.spi, &(Mfrc522FifoDataReg as u8))?;
    }

    reset_internal_memory()?;

    mfrc522.buffer.to_read = MFRC522_BUFSIZE;
    Ok(MFRC522_BUFSIZE as i32)
}

fn generate_random_id(args: &str) -> Result<i32, Error> {
    let mfrc522 = unsafe {
        match &mut G_MFRC522 {
            Some(mfrc) => mfrc,
            None => return Err(ENODEV),
        }
    };

    spi_write(
        &mut mfrc522.spi,
        &(Mfrc522CmdReg as u8),
        &mut (Mfrc522GenerateRandomId as u8),
    )?;
    Ok(MFRC522_BUFSIZE as i32)
}
