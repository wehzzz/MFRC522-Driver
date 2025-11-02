//! SPDX-License-Identifier: GPL-2.0
#![no_main]

use kernel::prelude::*;

module! {
    type: RustModule,
    name: "rust_module",
    author: "Rust for Linux Contributors",
    description: "Rust out-of-tree sample",
    license: "GPL",
}

struct RustModule;

impl kernel::Module for RustModule {
    fn init(_module: &'static ThisModule) -> Result<Self> {
        pr_info!("Rust module initialized\n");
        Ok(RustModule)
    }
}

impl Drop for RustModule {
    fn drop(&mut self) {
        pr_info!("Rust module cleaned up\n");
    }
}
