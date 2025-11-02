//! SPDX-License-Identifier: GPL-2.0
#![no_main]

pub mod internals;
use kernel::prelude::*;

module! {
    type: RustChrdev,
    name: "mfrc522",
    author: "Anton VELLA <anton.vella@epita.fr> & Martin LEVESQUE <martin.levesque@epita.fr>",
    description: "MFRC522 card reader driver",
    license: "GPL v2",
}

struct RustFile;

#[vtable]
impl file::Operations for RustFile {
    fn open(_shared: &(), _file: &file::File) -> Result {
        Ok(())
    }
}

struct RustChrdev {
    dev: internals::mfrc522::Mfrc522Device,
}

impl kernel::Module for RustChrdev {
    fn init(_module: &'static ThisModule) -> Result<Self> {
        pr_info!("Rust module initialized\n");
        Ok(RustChrdev {
            dev: internals::mfrc522::Mfrc522Device::default(),
        })
    }
}

impl Drop for RustChrdev {
    fn drop(&mut self) {
        pr_info!("Rust module cleaned up\n");
    }
}
