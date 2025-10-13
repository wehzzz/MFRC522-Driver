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
#include <linux/i2c.h>

#define DEVICE_NAME "mfrc522"
#define MFRC522_BUFSIZE 25

/* Structures:
 * - Device structure representing the MFRC522 device
 */
struct mfrc522_dev {
	struct cdev cdev;
	struct device *dev;
	struct i2c_client *client;
	char buf[MFRC522_BUFSIZE + 1]; /* trailing NUL */
	int debug;
};

#endif /* ! MFRC522_CORE_H */