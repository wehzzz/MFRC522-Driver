//! SPDX-License-Identifier: GPL-2.0
use core::ptr::addr_of_mut;
use kernel::prelude::*;

pub mod internals;

use crate::internals::spi;
use crate::internals::{
    Buffer, DriverRegistration, SpiDevice, SpiMethods, DEVICE_NAME, MFRC522_BUFSIZE,
};
use kernel::bindings;
use kernel::pr_info;

#[inline]
fn major(dev: bindings::dev_t) -> u32 {
    (dev >> 20) as u32
}

#[inline]
fn minor(dev: bindings::dev_t) -> u32 {
    (dev & 0xfffff) as u32
}

#[inline]
fn mkdev(major: u32, minor: u32) -> bindings::dev_t {
    (((major as u64) << 20) | (minor as u64))
        .try_into()
        .unwrap()
}

module! {
    type: Mfrc522Module,
    name: "mfrc522",
    author: "Anton VELLA <anton.vella@epita.fr>, Martin LEVESQUE <martin.levesque@epita.fr>",
    description: "MFRC522 card reader driver",
    license: "GPL v2",
}

struct Mfrc522Module {
    _registration: Pin<KBox<DriverRegistration>>,
}

struct Mfrc522Device {
    pub cdev: bindings::cdev,
    pub spi: SpiDevice,
    pub buffer: Buffer,
    pub debug: bool,
}

impl kernel::Module for Mfrc522Module {
    fn init(module: &'static ThisModule) -> Result<Self> {
        pr_info!("Initializing MFRC522 driver...\n");

        let registration = DriverRegistration::new_pinned::<Mfrc522Device>(module, DEVICE_NAME)?;

        pr_info!("MFRC522 SPI driver registered!\n");

        Ok(Mfrc522Module {
            _registration: registration,
        })
    }
}

impl Drop for Mfrc522Module {
    fn drop(&mut self) {
        pr_info!("Goodbye, GISTRE card !\n");
    }
}

static mut G_MFRC522: Option<KBox<Mfrc522Device>> = None;
static mut G_MAJOR: i32 = 0;

impl SpiMethods for Mfrc522Device {
    declare_spi_methods!(probe, remove);

    fn probe(spi_dev: SpiDevice) -> kernel::error::Result {
        pr_info!("Probing MFRC522 rfid card\n");

        unsafe {
            let mut dev: bindings::dev_t = 0;
            let ret = bindings::alloc_chrdev_region(&mut dev, 0, 1, DEVICE_NAME.as_char_ptr());

            if ret < 0 {
                pr_err!("MFRC522: failed to register device\n");
                return Err(Error::from_errno(ret));
            }

            G_MAJOR = major(dev) as i32;
            pr_info!("MFRC522: allocated major number for device {}\n", G_MAJOR);

            let mut mfrc522_dev = KBox::new(
                Mfrc522Device {
                    cdev: core::mem::zeroed(),
                    spi: spi_dev,
                    buffer: Buffer {
                        buffer: [0u8; MFRC522_BUFSIZE],
                        to_read: 0,
                    },
                    debug: false,
                },
                GFP_KERNEL,
            )?;

            let fops = bindings::file_operations {
                owner: addr_of_mut!(bindings::__this_module),
                open: Some(mfrc522_open),
                release: Some(mfrc522_release),
                read: Some(mfrc522_read),
                write: Some(mfrc522_write),
                ..core::mem::zeroed()
            };

            bindings::cdev_init(&mut mfrc522_dev.cdev, &fops);

            let ret = bindings::cdev_add(&mut mfrc522_dev.cdev, dev, 1);
            if ret < 0 {
                pr_err!("MFRC522: failed to add device to kernel\n");
                bindings::unregister_chrdev_region(dev, 1);
                return Err(Error::from_errno(ret));
            }

            /*
            if let Err(e) = print_version(&mut mfrc522_dev.spi) {
                pr_err!("Failed to read MFRC522 version\n");
                bindings::cdev_del(&mut mfrc522_dev.cdev);
                bindings::unregister_chrdev_region(dev, 1);
                return Err(e);
            }*/

            pr_info!("Hello, GISTRE card !\n");

            G_MFRC522 = Some(mfrc522_dev);
        }

        Ok(())
    }

    fn remove(_spi_dev: SpiDevice) -> kernel::error::Result {
        pr_info!("Removing MFRC522 rfid card driver\n");

        unsafe {
            if let Some(mut mfrc522) = G_MFRC522.take() {
                bindings::cdev_del(&mut mfrc522.cdev);
                let dev = mkdev(G_MAJOR as u32, 0);
                bindings::unregister_chrdev_region(dev, 1);
            }
        }

        Ok(())
    }
}

unsafe extern "C" fn mfrc522_open(
    _inode: *mut bindings::inode,
    _file: *mut bindings::file,
) -> core::ffi::c_int {
    0
}

unsafe extern "C" fn mfrc522_release(
    _inode: *mut bindings::inode,
    _file: *mut bindings::file,
) -> core::ffi::c_int {
    0
}

unsafe extern "C" fn mfrc522_read(
    _file: *mut bindings::file,
    _buf: *mut u8,
    _len: usize,
    _off: *mut bindings::loff_t,
) -> isize {
    0 as isize
}

unsafe extern "C" fn mfrc522_write(
    _file: *mut bindings::file,
    _buf: *const u8,
    _len: usize,
    _off: *mut bindings::loff_t,
) -> isize {
    0 as isize
}
