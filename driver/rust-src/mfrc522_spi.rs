use kernel::{
    error::{Error, Result},
    pr_err,
};

use crate::internals::spi::{Spi, SpiDevice};

const READ_MODE: u8 = 0x80;
const MASK: u8 = 0x7E;

pub(crate) fn spi_write(spi: &mut SpiDevice, reg: &u8, data: &mut u8) -> Result<(), Error> {
    let tx: [u8; 2] = [(*reg << 1) & MASK, *data];
    Spi::write(spi, &tx).map_err(|err| {
        pr_err!("MFRC522: failed to write data to register {:#04x}\n", reg);
        err
    })
}

pub(crate) fn spi_read(spi: &mut SpiDevice, reg: &u8) -> Result<u8, Error> {
    let tx_header: [u8; 1] = [((*reg << 1) & MASK) | READ_MODE];
    let mut rx_data: [u8; 1] = [0; 1];

    Spi::write(spi, &tx_header).map_err(|err| {
        pr_err!("MFRC522: failed to send read header for reg {:#04x}\n", reg);
        err
    })?;

    Spi::read(spi, &mut rx_data).map_err(|err| {
        pr_err!(
            "MFRC522: failed to read register data for reg {:#04x}\n",
            reg
        );
        err
    })?;

    Ok(rx_data[0])
}
