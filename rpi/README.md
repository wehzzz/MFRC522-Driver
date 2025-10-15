# Compile Kernel for RPI
## Download sources
```sh
git clone --depth=1 https://github.com/raspberrypi/linux.git
```

## Install required dependencies and toolchain
```sh
sudo apt install bc bison flex libssl-dev make libc6-dev libncurses5-dev
sudo apt install crossbuild-essential-armhf
```

## Build configuration
```sh
cd linux
KERNEL=kernel7
make -j12 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2709_defconfig
make -j12 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage modules dtbs
```

## Install Kernel Headers
```sh
make -j12 headers_install INSTALL_HDR_PATH=/usr/src/linux-headers-6.12.51-v7+
sudo ln -s /usr/src/linux-headers-6.12.51-v7+ /lib/modules/6.12.51-v7+/build
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
```
mkdir mnt
mkdir mnt/boot
mkdir mnt/root
sudo mount /dev/sde1 mnt/boot
sudo mount /dev/sde2 mnt/root
```

### Install
```
sudo make -j12 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=mnt/root modules_install
sudo cp mnt/boot/$KERNEL.img mnt/boot/$KERNEL-backup.img
sudo cp arch/arm/boot/zImage mnt/boot/$KERNEL.img
sudo cp arch/arm/boot/dts/broadcom/*.dtb mnt/boot/
sudo cp arch/arm/boot/dts/overlays/*.dtb* mnt/boot/overlays/
sudo cp arch/arm/boot/dts/overlays/README mnt/boot/overlays/
sudo umount mnt/boot
sudo umount mnt/root
```

### Edit Device Tree
```
&spi0 {
	pinctrl-names = "default";
	pinctrl-0 = <&spi0_pins &spi0_cs_pins>;
	cs-gpios = <&gpio 8 1>, <&gpio 7 1>;
	status = "okay";
	
	mfrc522@0 {
        compatible = "nxp,mfrc522";
        reg = <0>;
        spi-max-frequency = <4000000>;
        status = "okay";
    };

	spidev1: spidev@1{
		compatible = "spidev";
		reg = <1>;	/* CE1 */
		#address-cells = <1>;
		#size-cells = <0>;
		spi-max-frequency = <125000000>;
	};
};
```
then
```
make -j12 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- dtbs
```

### Config
Put thos lines in the config.txt present in the SD card:
```
enable_uart=1
arm_64bit=0
uart_2ndstage=1
kernel=kernel7.img
device_tree=bcm2710-rpi-3-b-plus.dtb
dtparam=spi=on
```

# SSH

I connected mi RPI3b+ to my computer via ethernet. Make sure to share your wifi connection.
My ethernet's ip is `192.168.137.1` so i just did the following commands on the RPI:
```
sudo ip addr flush dev eth0
sudo ip addr add 192.168.137.2/24 dev eth0
sudo ip link set eth0 up
```

then you can use scp to transfer your file to the RPI
```
scp mfrc522.ko pi@192.168.137.2:/home/pi/
```