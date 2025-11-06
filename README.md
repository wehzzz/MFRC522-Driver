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

## Basic steps (0, 1 & 2 )
For the basic steps, we have implemented the functions as follows:

`env/emu.env` -> Environment variables required to compile the driver and launch QEMU. You must export the variables using, for example, `. env/emu.env` before compiling or launching QEMU.

`groupe7/src/mfrc522_core.*` -> contains the functions that allow interaction with our device. That is, initialisation, deletion of the device, and file operations such as `write`, `read`, `open`, `release`.

`groupe7/src/mfrc522_commands.*` -> contains the sub-functions that allow us to implement the expected operations of `mem_write:{len}:{data}`, `gen_rand_id`, `mem_read`. That is, the various writes and reads in the FIFO/internal memory of the MFRC522.

`groupe7/src/Makefile` -> We compile our driver with the emulator's kernel headers, which we retrieve via environment variables. We also add the `mfrc522.h` header to the included sources to access the various functions and registers available.

You can run `make emu` and then `make run` to compile and run the driver with QEMU.
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

After making this change, we were able to boot and set up an SSH tunnel to send our module. We did not encounter any particular difficulties in implementing the driver apart from switching from ragmap to SPI.

### Step 6.
### Step 7.