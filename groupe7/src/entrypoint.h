#ifndef GISTRE_CARD_H
#define GISTRE_CARD_H

#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/regmap.h>
#include <linux/errno.h>
#include "mfrc522.h"

#define MFRC522_BUFSIZE 25

/* Structures:
 * - Device structure representing the MFRC522 device
 */
struct card_dev {
	struct cdev cdev;
	struct device *dev;
	struct mfrc522_dev *mfrc522;
	struct regmap *regmap;
	char buf[MFRC522_BUFSIZE];
};

#endif /* ! GISTRE_CARD_H */