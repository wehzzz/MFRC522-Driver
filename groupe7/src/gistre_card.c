#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/slab.h>

MODULE_AUTHOR("Anton VELLA <anton.vella@epita.fr>");
MODULE_AUTHOR("Martin LEVESQUE <martin.levesque@epita.fr>");
MODULE_DESCRIPTION("MFRC522 card reader driver");
MODULE_LICENSE("GPL v2");

/* Structures:
 * - Device structure representing the MFRC522 device
 */
struct mfrc522_dev {
	struct cdev cdev;
};

/* Prototypes for file operations callbacks:
 * - read/write   : data transfer between user space and the device
 * - open/release : manage access to the device
 */
static ssize_t read(struct file *, char __user *, size_t, loff_t *);
static ssize_t write(struct file *, const char __user *, size_t, loff_t *);
static int open(struct inode *, struct file *);
static int release(struct inode *, struct file *);

/* Global variables for the module:
 * - g_major   : dynamically assigned major number for the device
 * - g_mfrc522 : pointer to the device structure instance
 * - g_fops    : table of file operations linked to our callbacks
 */
static int g_major;
static struct mfrc522_dev *g_mfrc522;
static struct file_operations g_fops = {
	.owner = THIS_MODULE,
	.read = read,
	.write = write,
	.open = open,
	.release = release,
};

__init static int gistre_card_init(void)
{
	pr_info("Hello, GISTRE card !\n");
	return 0;
}

__exit static void gistre_card_exit(void)
{
}

module_init(gistre_card_init);
module_exit(gistre_card_exit);
