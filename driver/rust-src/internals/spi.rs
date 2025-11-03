// SPDX-License-Identifier: GPL-2.0

use core::ffi;
use core::pin::Pin;
use kernel::alloc::kbox::KBox;
use kernel::bindings;
use kernel::error::{Error, Result};
use kernel::prelude::GFP_KERNEL;
use kernel::str::CStr;
use kernel::ThisModule;

#[derive(Clone, Copy)]
pub struct SpiDevice(*mut bindings::spi_device);

impl SpiDevice {
    pub unsafe fn from_ptr(dev: *mut bindings::spi_device) -> Self {
        SpiDevice(dev)
    }

    pub fn to_ptr(&mut self) -> *mut bindings::spi_device {
        self.0
    }
}

pub struct DriverRegistration {
    this_module: &'static crate::ThisModule,
    registered: bool,
    name: &'static CStr,
    spi_driver: bindings::spi_driver,
}

pub struct ToUse {
    pub probe: bool,
    pub remove: bool,
    pub shutdown: bool,
}

pub const USE_NONE: ToUse = ToUse {
    probe: false,
    remove: false,
    shutdown: false,
};

pub trait SpiMethods {
    const TO_USE: ToUse;

    fn probe(mut _spi_dev: SpiDevice) -> Result {
        Ok(())
    }

    fn remove(mut _spi_dev: SpiDevice) -> Result {
        Ok(())
    }

    fn shutdown(mut _spi_dev: SpiDevice) {}
}

/// Populate the TO_USE field in the `SpiMethods` implementer
#[macro_export]
macro_rules! declare_spi_methods {
    () => {
        const TO_USE: $crate::spi::ToUse = $crate::spi::USE_NONE;
    };
    ($($method:ident),+) => {
        const TO_USE: $crate::spi::ToUse = $crate::spi::ToUse {
            $($method: true),+,
            ..$crate::spi::USE_NONE
        };
    };
}

impl DriverRegistration {
    fn new(this_module: &'static crate::ThisModule, name: &'static CStr) -> Self {
        DriverRegistration {
            this_module,
            name,
            registered: false,
            spi_driver: unsafe { core::mem::zeroed() },
        }
    }

    pub fn new_pinned<T: SpiMethods>(
        this_module: &'static crate::ThisModule,
        name: &'static CStr,
    ) -> Result<Pin<KBox<Self>>> {
        let mut registration = Pin::from(KBox::new(Self::new(this_module, name), GFP_KERNEL)?);
        registration.as_mut().register::<T>()?;
        Ok(registration)
    }

    unsafe extern "C" fn probe_wrapper<T: SpiMethods>(
        spi_dev: *mut bindings::spi_device,
    ) -> ffi::c_int {
        let spi_dev = unsafe { SpiDevice::from_ptr(spi_dev) };
        match T::probe(spi_dev) {
            Ok(_) => 0,
            Err(e) => e.to_errno(),
        }
    }

    unsafe extern "C" fn remove_wrapper<T: SpiMethods>(spi_dev: *mut bindings::spi_device) {
        let spi_dev = unsafe { SpiDevice::from_ptr(spi_dev) };
        // Même si T::remove() renvoie Result, on ignore le retour (pas de code d’erreur transmis)
        let _ = T::remove(spi_dev);
    }

    unsafe extern "C" fn shutdown_wrapper<T: SpiMethods>(spi_dev: *mut bindings::spi_device) {
        let spi_dev = unsafe { SpiDevice::from_ptr(spi_dev) };
        T::shutdown(spi_dev)
    }

    pub fn register<T: SpiMethods>(self: Pin<&mut Self>) -> Result {
        let this = unsafe { self.get_unchecked_mut() };
        if this.registered {
            return Err(Error::from_errno(-(bindings::EINVAL as i32)));
        }

        this.spi_driver.driver.name = this.name.as_ptr() as *const u8;

        // Casts direct vers les bons types de fonction
        this.spi_driver.probe = if T::TO_USE.probe {
            Some(
                Self::probe_wrapper::<T>
                    as unsafe extern "C" fn(*mut bindings::spi_device) -> ffi::c_int,
            )
        } else {
            None
        };

        this.spi_driver.remove = if T::TO_USE.remove {
            Some(Self::remove_wrapper::<T> as unsafe extern "C" fn(*mut bindings::spi_device))
        } else {
            None
        };

        this.spi_driver.shutdown = if T::TO_USE.shutdown {
            Some(Self::shutdown_wrapper::<T> as unsafe extern "C" fn(*mut bindings::spi_device))
        } else {
            None
        };

        let res = unsafe {
            bindings::__spi_register_driver(this.this_module.as_ptr(), &mut this.spi_driver)
        };

        match res {
            0 => {
                this.registered = true;
                Ok(())
            }
            err => Err(Error::from_errno(err)),
        }
    }
}

impl Drop for DriverRegistration {
    fn drop(&mut self) {
        unsafe { bindings::driver_unregister(&mut self.spi_driver.driver) }
    }
}

unsafe impl Sync for DriverRegistration {}
unsafe impl Send for DriverRegistration {}

pub struct Spi;

impl Spi {
    pub fn write_then_read(dev: &mut SpiDevice, tx_buf: &[u8], rx_buf: &mut [u8]) -> Result {
        let res = unsafe {
            bindings::spi_write_then_read(
                dev.to_ptr(),
                tx_buf.as_ptr() as *const ffi::c_void,
                tx_buf.len() as ffi::c_uint,
                rx_buf.as_mut_ptr() as *mut ffi::c_void,
                rx_buf.len() as ffi::c_uint,
            )
        };

        match res {
            0 => Ok(()),
            err => Err(Error::from_errno(err)),
        }
    }

    #[inline]
    pub fn write(dev: &mut SpiDevice, tx_buf: &[u8]) -> Result {
        Spi::write_then_read(dev, tx_buf, &mut [0u8; 0])
    }

    #[inline]
    pub fn read(dev: &mut SpiDevice, rx_buf: &mut [u8]) -> Result {
        Spi::write_then_read(dev, &[0u8; 0], rx_buf)
    }
}
