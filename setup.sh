#!/bin/sh

echo "Export ENV variables"
source groupe7/emu.env
cd groupe7/src/
echo "Compiling the module"
make modules

if [ "$1" = "run" ]; then
    cd -
    cd gistre26-dril-sdk/dril-sdk/buildroot-dril-sdk/
    ./start-qemu.sh
fi
