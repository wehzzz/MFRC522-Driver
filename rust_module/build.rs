use std::process::Command;
use std::path::Path;

fn main() {
    // Build staticlib
    Command::new("cargo")
        .args(&["build", "--release", "--target", "armv7-unknown-linux-gnueabihf"])
        .status()
        .unwrap();

    // Copy the .o to the kernel module folder
    let src = Path::new("target/armv7-unknown-linux-gnueabihf/release/librust_module.a");
    let dst = Path::new("rust_module.o");
    std::fs::copy(src, dst).unwrap();
}
