# MFRC522-Driver

## AUTHORS:

- anton.vella
- martin.levesque

## Introduction

This document contains information on the project to create a driver for an MFRC522 RFID card reader.

At the root of the project, you will find a Makefile with the following rules:

- `make emu` -> driver compatible with emulator
- `make rpi` -> driver compatible with RPI 3b+
- `make rust` -> Rust driver compatible with RPI 3b+
- `make run` -> launch QEMU
- `make clean`

These rules allow you to compile the different steps of the project by exporting the environment variables correctly.

> [!NOTE]
> You won't be able to make rust and rpi from the get go. You will need to follow the readme found in `driver/Readme.md`

## Basic steps (0, 1 & 2 )

For the basic steps, we have implemented the functions as follows:

`env/emu.env` -> Environment variables required to compile the driver and launch QEMU. You must export the variables using, for example, `. env/emu.env` before compiling or launching QEMU.

`groupe7/src/mfrc522_core.*` -> contains the functions that allow interaction with our device. That is, initialisation, deletion of the device, and file operations such as `write`, `read`, `open`, `release`.

`groupe7/src/mfrc522_commands.*` -> contains the sub-functions that allow us to implement the expected operations of `mem_write:{len}:{data}`, `gen_rand_id`, `mem_read`. That is, the various writes and reads in the FIFO/internal memory of the MFRC522.

`groupe7/src/Makefile` -> We compile our driver with the emulator's kernel headers, which we retrieve via environment variables. We also add the `mfrc522.h` header to the included sources to access the various functions and registers available.

In order to build and run the driver you can make the following commands at the root of this project:
```sh
make emu && make run
```

or

```sh
source env/emu.env
cd groupe7/src
make modules
cd ../../gistre26-dril-sdk/buildroot-dril-sdk
./start-qemu.sh
```

## Bonus steps

### Step 2.

For this bonus, we have added support for the `debug:{on|off}` command in the files `groupe7/src/mfrc522_commands.*`. This allows information read and written when calling the `mem_write` and `mem_read` commands to be displayed in the kernel.

### Step 4.

For bonus 4, we have added a function that allows us to read the MFRC522 `version` field in the device tree using the `of_property_read_u32()` function. This allows us to read the properties that were parsed in the device tree when the emulator was launched.

### Step 5.

For the bonus of switching to Raspberry, we had to recompile a kernel.

We encountered several problems during this step. Despite the guide on the official website

At first, we had a faulty card that would not boot our compiled kernel, so we wasted a lot of time at that point.

After changing the card, we didn't encounter any particular problems. We changed the device tree to add our card to the SPI bus.

When launching the card, we encountered a `ZRAM0.swap` error that prevented us from booting our RPI correctly. To fix the problem, we had to change the kernel configuration with menuconfig and enable the option:

```
Device Driver -> Block Devices -> Compressed Ram Block Device Support[y].
```

After making this change, we were able to boot and set up an SSH tunnel to send our module. We did not encounter any particular difficulties in implementing the driver apart from switching from regmap to SPI.

### Step 6.

The implemetation of this step can be found in `driver/c-src/mfrc522_spi.c`. We decided to keep a simple approach regarding this step providing an API to write and read a byte at a time enabling us to keep the same architecture as the qemu one. Using the `<linux/spi.h>` API with `spi_write` to write and `spi_write_then_read` to read.

Issues encountered during this step were that we intially started with I2C but could'nt manage to get the mfrc522 to ACK so we decided to switch to spi and did not ecountered any other issues.

### Step 7.

For this bonus we had to setup a Rust working environment installing the right toolchains that was accepted by the kernel so that the CONFIG_RUST_AVAILABLE kconfig variable is set.

Then configurate the kernel so that the Rust support is enabled and the core and bindings crate are built and linked when building the kernel.

After that we needed to reimplement the mfrc522 module in rust using the core and bindings crates provided by the kernel to be able to interact with the kernel APIs.

Issues encountered during this step:

- Learning we had to enable the Rust support as it was not enabled by default like we though.
- Managing to set the right settings on the kernel. As we initially built it with the 32bit ARM architecture we were not supposed to be able to set it so we tried to modify the kconfig by hand which led to the kernel not building in it's entirety causing a "no symtab" error. We took quite some time fixing this issue as we were quite stuborn with the usage of the 32bit ARM architecture. Fixed by switching to 64bit ARM.
- We also had a segfault at some point because we registered the fops with a variable that was created inside a function which meant that this variable containing the pointers to write/read functions was detroyed at the end of this scope. This as a behaviour is quite funny because it means that the kernel keeps the pointer to the fops and not only to the pointer to the functions that we provide.
- Once we started really working on the driver we had some issues with our includes even with files inside of the crate. This was due to the fact that when building we were providing all our files as object files but actually when building rust into an object file one object file is one crate which led in our case to some weird dependencies problem.
