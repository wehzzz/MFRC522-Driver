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

# SSH

I connected mi RPI3b+ to my computer via ethernet. Make sure to share your wifi connection.
My ethernet's ip is `192.168.137.1` so i just did the following commands on the RPI:
```
ip addr flush dev eth0
ip addr add 192.168.137.2/24 dev eth0
ip link set eth0 up
```

then you can use scp to transfer your file to the RPI
```
scp MFRC522-Driver/rpi/src/test.ko  pi@192.168.137.2:/home/pi/
```