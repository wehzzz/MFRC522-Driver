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
#include <linux/types.h>
#include "mfrc522.h"

#define MFRC522_BUFSIZE 25
#define DEVICE_NAME "card"

/*
 * Structures
 */
struct buffer {
	char buf[MFRC522_BUFSIZE];
	size_t to_read;
};

struct card_dev {
	struct cdev cdev;
	struct device *dev;
	struct mfrc522_dev *mfrc522;
	struct regmap *regmap;
	struct buffer buffer;
	bool debug;
};

#endif /* ! MFRC522_CORE_H */