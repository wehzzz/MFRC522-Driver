#ifndef MFRC522_CORE_H
#define MFRC522_CORE_H

#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/regmap.h>
#include <linux/errno.h>
#include <linux/of.h>
#include <linux/spi/spi.h>

#define DEVICE_NAME "mfrc522"
#define MFRC522_BUFSIZE 25

/*
 * Device tree representation of the MFRC522 emulated device
 */
#define MFRC522_PROP_VERSION "version"

/*
 * Register map
 */
#define MFRC522_CMDREG 0x01
#define MFRC522_CMDREG_CMD 0x0F
#define MFRC522_CMDREG_SOFTPWRDOWN 0x10
#define MFRC522_CMDREG_RCVOFF 0x20
#define MFRC522_FIFODATAREG 0x09
#define MFRC522_FIFOLEVELREG 0x0A
#define MFRC522_FIFOLEVELREG_LEVEL 0x7F
#define MFRC522_FIFOLEVELREG_FLUSH 0x80

/*
 * Command set
 */
#define MFRC522_IDLE 0x0
#define MFRC522_MEM 0x1
#define MFRC522_GENERATERANDOMID 0x2
#define MFRC522_NOCMDCHANGE 0x7
#define MFRC522_SOFTRESET 0xF

/* 
 * Structures
 */
struct buffer {
	char buf[MFRC522_BUFSIZE];
	size_t to_read;
};

struct mfrc522_dev {
	struct cdev cdev;
	struct device *dev;
	struct spi_device *spi;
	struct buffer buffer;
	int debug;
};

#endif /* ! MFRC522_CORE_H */