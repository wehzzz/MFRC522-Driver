.PHONY: all rpi emu clean run rust

all:
	@echo "Usage:"
	@echo "  make rpi    - build module for Raspberry Pi"
	@echo "  make emu    - build module for emulator"
	@echo "  make clean  - clean all builds"
	@echo "  make run    - run the QEMU emulator"

rpi:
	@echo "Building module for RPI ..."
	. env/rpi.env && make -C rpi/src modules

rust:
	@echo "Building module for Rust ..."
	. env/rust.env && make -C rpi/linux M=$(PWD)/rust_module \
    ARCH=arm \
    CROSS_COMPILE=arm-linux-gnueabihf- \
    LLVM=1 \
    CONFIG_RUST=y \
	modules

emu:
	@echo "Building module for QEMU ..."
	. env/emu.env && make -C groupe7/src modules

clean:
	@echo "Cleaning all builds ..."
	. env/rpi.env && make -C rpi/src clean
	. env/emu.env && make -C groupe7/src clean

run:
	. env/emu.env && ./gistre26-dril-sdk/dril-sdk/buildroot-dril-sdk/start-qemu.sh
