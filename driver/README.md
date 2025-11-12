# Rust Kernel - RPI 3b+

Here are the different steps to compile and obtain a functional kernel that supports Rust.

**Warning: Rust is only supported on 64-bit kernels.**

## Build dependencies

```sh
sudo apt install bc bison flex libssl-dev make libc6-dev libncurses5-dev
sudo apt install crossbuild-essential-arm64
```

## Rust toolchain

You can follow this guide to obtain the necessary tools to make rust available in your kernel:

- https://docs.kernel.org/6.14/rust/quick-start.html

## Download sources

```sh
git clone --depth=1 https://github.com/raspberrypi/linux.git
```

## Build

```sh
cd linux
KERNEL=kernel8
make -j12 LLVM=1 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- bcm2711_defconfig
```

Then you'll have to change de kernel default configuration by running:

```sh
make -j12 LLVM=1 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- menuconfig
```

Rust support (`CONFIG_RUST`) needs to be enabled in the General setup menu. The option is only shown if a suitable Rust toolchain is found (see above), as long as the other requirements are met. You may also need to turn off module versioning support to be able to see the Rust support option, you can find it in the Enable loadable module support.

Afterwars you have to edit your device tree in order to add support for the MFRC522:

```yaml
&spi0 {
	pinctrl-names = "default";
	pinctrl-0 = <&spi0_pins &spi0_cs_pins>;
	cs-gpios = <&gpio 8 1>, <&gpio 7 1>;
	status = "okay";

	mfrc522: mfrc522@0{
        compatible = "nxp,mfrc522";
        reg = <0>;
        spi-max-frequency = <4000000>;
        status = "okay";
    };

	spidev1: spidev@1{
		compatible = "spidev";
		reg = <1>;
		spi-max-frequency = <125000000>;
	};
};
```

Then, add the following lines to `linux/rust/bindings/bindings_helper.h` to let Bindgen know that these additional bindings should be generated for Rust.

```c
#include <linux/spi/spi.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
```

And run:

```sh
make -j12 LLVM=1 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- rust/bindings_generated.rs
```

Finally, compile everything needed to install the kernel on the Raspberry Pi:

```sh
make -j12 LLVM=1 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- Image modules dtbs
```

## Install the kernel

### Find your boot media

First, run lsblk. Then, connect your boot media. Run lsblk again; the new device represents your boot media. You should see output similar to the following:

```
sdb
   sdb1
   sdb2
```

If sdb represents your boot media, sdb1 represents the the FAT32-formatted boot partition and sdb2 represents the (likely ext4-formatted) root partition.

First, mount these partitions as mnt/boot and mnt/root, adjusting the partition letter to match the location of your boot media:

```sh
mkdir mnt
mkdir mnt/boot
mkdir mnt/root
sudo mount /dev/sde1 mnt/boot
sudo mount /dev/sde2 mnt/root
```

### Install

```sh
sudo make -j12 LLVM=1 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- INSTALL_MOD_PATH=mnt/root modules_install
sudo cp mnt/boot/$KERNEL.img mnt/boot/$KERNEL-backup.img
sudo cp arch/arm64/boot/Image mnt/boot/$KERNEL.img
sudo cp arch/arm64/boot/dts/broadcom/*.dtb mnt/boot/
sudo cp arch/arm64/boot/dts/overlays/*.dtb* mnt/boot/overlays/
sudo cp arch/arm64/boot/dts/overlays/README mnt/boot/overlays/
sudo umount mnt/boot
sudo umount mnt/root
```

### Config

Put thoses lines in the config.txt present in the SD card:

```
enable_uart=1
arm_64bit=0
uart_2ndstage=1
kernel=kernel7.img
device_tree=bcm2710-rpi-3-b-plus.dtb
dtparam=spi=on
```

# Compile your driver

You’ll first need to install the kernel headers corresponding to your kernel version.

```sh
sudo make -j12 LLVM=1 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules_install
```

Then you'll be able to run:

```sh
make modules
```

# SSH

Connect your Raspberry Pi 3B+ to your computer via Ethernet.
Make sure to share your Wi-Fi connection with the Ethernet interface.

As an example, if your computer’s Ethernet IP address is `192.168.137.1`, you can run the following commands on the Raspberry Pi in order to create a local network:

```sh
sudo ip addr flush dev eth0
sudo ip addr add 192.168.137.2/24 dev eth0
sudo ip link set eth0 up
```

then you can use scp to transfer your file to the RPI

```sh
scp mfrc522.ko pi@192.168.137.2:/home/pi/
```
