#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <mfrc522.h>
#include <linux/uaccess.h>

#define MFRC522_BUFSIZE 25

MODULE_AUTHOR("Anton VELLA <anton.vella@epita.fr>");
MODULE_AUTHOR("Martin LEVESQUE <martin.levesque@epita.fr>");
MODULE_DESCRIPTION("MFRC522 card reader driver");
MODULE_LICENSE("GPL v2");
MODULE_SOFTDEP("pre: mfrc522_emu");

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

/* Prototypes for file operations callbacks:
 * - read/write   : data transfer between user space and the device
 * - open/release : manage access to the device
 */
static ssize_t mfrc522_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t mfrc522_write(struct file *, const char __user *, size_t,
			     loff_t *);
static int mfrc522_open(struct inode *, struct file *);
static int mfrc522_release(struct inode *, struct file *);

/* Global variables for the module:
 * - g_major   : dynamically assigned major number for the device
 * - g_mfrc522 : pointer to the device structure instance
 * - g_fops    : table of file operations linked to our callbacks
 */
static int g_major;
static struct card_dev *g_mfrc522;
static struct file_operations g_fops = {
	.owner = THIS_MODULE,
	.read = mfrc522_read,
	.write = mfrc522_write,
	.open = mfrc522_open,
	.release = mfrc522_release,
};

/*
 * Function declaration
 */
__init static int gistre_card_init(void)
{
	dev_t dev;
	int ret = 0;
	char *name = "card";

	/* Step 1: dynamically allocate a major number */
	ret = alloc_chrdev_region(&dev, 0, 1, name);
	if (ret < 0) {
		pr_err("MFRC522: failed to register device\n");
		goto end;
	}

	g_major = MAJOR(dev);
	pr_info("MFRC522: allocated major number for device %d\n", g_major);

	/* Step 2: allocate memory for the device structure */
	g_mfrc522 = kmalloc(sizeof(*g_mfrc522), GFP_KERNEL);
	if (!g_mfrc522) {
		pr_err("MFRC522: failed to allocate memory for device\n");
		ret = -ENOMEM;
		goto unregister_dev;
	}

	/* Step 3: initialize the cdev structure and add it to the kernel */
	cdev_init(&g_mfrc522->cdev, &g_fops);
	g_mfrc522->cdev.owner = THIS_MODULE;
	memset(g_mfrc522->buf, 0, MFRC522_BUFSIZE);
	ret = cdev_add(&g_mfrc522->cdev, dev, 1);
	if (ret < 0) {
		pr_err("MFRC522: failed to add device to kernel\n");
		goto free_dev;
	}

	g_mfrc522->dev = mfrc522_find_dev();
	if (!g_mfrc522->dev) {
		pr_err("MFRC522: could not find platform device\n");
		return -ENODEV;
	}
	g_mfrc522->mfrc522 = dev_to_mfrc522(g_mfrc522->dev);
	if (!g_mfrc522->mfrc522) {
		pr_err("MFRC522: could not find platform device\n");
		return -ENODEV;
	}
	g_mfrc522->regmap = mfrc522_get_regmap(g_mfrc522->mfrc522);
	if (!g_mfrc522->regmap) {
		pr_err("MFRC522: could not find regmap\n");
		return -ENODEV;
	}

	pr_info("Hello, GISTRE card !\n");
	goto end;

free_dev:
	kfree(g_mfrc522);
unregister_dev:
	unregister_chrdev_region(dev, 1);
end:
	return ret;
}
module_init(gistre_card_init);

__exit static void gistre_card_exit(void)
{
	dev_t dev;

	cdev_del(&g_mfrc522->cdev);

	dev = MKDEV(g_major, 0);
	kfree(g_mfrc522);

	unregister_chrdev_region(dev, 1);
	pr_info("Goodbye, GISTRE card !\n");
}
module_exit(gistre_card_exit);

static ssize_t mfrc522_read(struct file *file, char __user *buf, size_t len,
			    loff_t *off)
{
	return 0;
}

static ssize_t mfrc522_write(struct file *file, const char __user *buf,
			     size_t len, loff_t *off)
{
	ssize_t ret;
	char *kbuf = kmalloc(len + 1, GFP_KERNEL);
	struct card_dev* mfrc522 = (struct card_dev*) file->private_data;
	(void)off;
	if (!kbuf)
		return -ENOMEM;

	memset(kbuf, 0, len + 1);
	ret = copy_from_user(kbuf, buf, len);
	if (ret != 0) {
		pr_err("MFRC522: failed to copy data from user\n");
		kfree(kbuf);
		return -EFAULT;
	}

	pr_info("%s\n", kbuf);
	kfree(kbuf);
	return len;
}

static int mfrc522_open(struct inode *inode, struct file *file)
{
	unsigned i_major;
	unsigned i_minor;

	i_major = imajor(inode);
	if (i_major != g_major) {
		pr_err("MFRC522: when opening node, found invalid major number %d (expected %d)\n",
		       i_major, g_major);
		return -ENODEV;
	}

	i_minor = iminor(inode);
	if (i_minor != 0) {
		pr_err("MFRC522: when opening node, found invalid nonzero minor\n");
		return -ENODEV;
	}

	file->private_data = g_mfrc522;
	return 0;
}

static int mfrc522_release(struct inode *inode, struct file *file)
{
	(void)inode;
	(void)file;
	return 0;
}
