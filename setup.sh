#!/bin/sh

cd gistre26-dril-sdk/dril-sdk/
echo "Export ENV variables"
export PATH=$(pwd)/buildroot-dril-sdk/aarch64-buildroot-linux-gnu_sdk-buildroot/bin:"$PATH"
export KDIR=$(pwd)/buildroot-dril-sdk/linux-build
export KBUILD_EXTRA_SYMBOLS=$(pwd)/mfrc522_emu-sdk/Module.symvers
export ARCH=arm64
export CROSS_COMPILE=aarch64-buildroot-linux-gnu-
cd -
cd groupe7/src/
echo "Compiling the module"
make modules

if [ "$1" = "run" ]; then
    cd -
    cd gistre26-dril-sdk/dril-sdk/buildroot-dril-sdk/
    ./start-qemu.sh
fi
