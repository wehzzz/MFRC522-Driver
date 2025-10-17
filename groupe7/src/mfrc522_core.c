#include "mfrc522_core.h"

#include "mfrc522_commands.h"
#include "mfrc522_utils.h"

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Anton VELLA <anton.vella@epita.fr>");
MODULE_AUTHOR("Martin LEVESQUE <martin.levesque@epita.fr>");
MODULE_DESCRIPTION("MFRC522 card reader driver");
MODULE_SOFTDEP("pre: mfrc522_emu");

static ssize_t mfrc522_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t mfrc522_write(struct file *, const char __user *, size_t,
			     loff_t *);
static int mfrc522_open(struct inode *, struct file *);
static int mfrc522_release(struct inode *, struct file *);

/* 
 * Global variables for the module
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

static ssize_t mfrc522_read(struct file *file, char __user *buf, size_t len,
			    loff_t *off)
{
	struct card_dev *mfrc522;
	int remaining_off;

	(void)off;

	mfrc522 = (struct card_dev *)file->private_data;
	if (mfrc522->buffer.to_read <= 0)
		return 0;

	remaining_off = MFRC522_BUFSIZE - mfrc522->buffer.to_read;
	len = min(len, mfrc522->buffer.to_read);

	if (copy_to_user(buf, mfrc522->buffer.buf + remaining_off, len)) {
		pr_err("MFRC522: When copying data to user, failed memory copy verification\n");
		return -EFAULT;
	}
	mfrc522->buffer.to_read -= len;

	return len;
}

static ssize_t mfrc522_write(struct file *file, const char __user *buf,
			     size_t len, loff_t *off)
{
	int ret;
	struct card_dev *mfrc522;
	char *kbuf;

	(void)off;

	mfrc522 = (struct card_dev *)file->private_data;
	kbuf = kmalloc(len + 1, GFP_KERNEL);

	if (!kbuf)
		return -ENOMEM;

	memset(kbuf, 0, len + 1);

	if (copy_from_user(kbuf, buf, len) != 0) {
		pr_err("MFRC522: failed to copy data from user\n");
		kfree(kbuf);
		return -EFAULT;
	}

	if ((ret = command_handle(mfrc522, kbuf)) < 0) {
		kfree(kbuf);
		return ret;
	}

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

__init static int gistre_card_init(void)
{
	dev_t dev;
	int ret = 0;

	ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
	if (ret < 0) {
		pr_err("MFRC522: failed to register device\n");
		goto end;
	}

	g_major = MAJOR(dev);
	pr_info("MFRC522: allocated major number for device %d\n", g_major);

	g_mfrc522 = kmalloc(sizeof(*g_mfrc522), GFP_KERNEL);
	if (!g_mfrc522) {
		pr_err("MFRC522: failed to allocate memory for device\n");
		ret = -ENOMEM;
		goto unregister_dev;
	}

	cdev_init(&g_mfrc522->cdev, &g_fops);
	g_mfrc522->cdev.owner = THIS_MODULE;
	g_mfrc522->debug = false;
	g_mfrc522->buffer.to_read = 0;
	memset(g_mfrc522->buffer.buf, 0, MFRC522_BUFSIZE);
	ret = cdev_add(&g_mfrc522->cdev, dev, 1);
	if (ret < 0) {
		pr_err("MFRC522: failed to add device to kernel\n");
		goto free_dev;
	}

	g_mfrc522->dev = mfrc522_find_dev();
	if (!g_mfrc522->dev) {
		pr_err("MFRC522: could not find platform device\n");
		goto error_handle;
	}
	g_mfrc522->mfrc522 = dev_to_mfrc522(g_mfrc522->dev);
	if (!g_mfrc522->mfrc522) {
		pr_err("MFRC522: could not find platform device\n");
		goto error_handle;
	}
	g_mfrc522->regmap = mfrc522_get_regmap(g_mfrc522->mfrc522);
	if (!g_mfrc522->regmap) {
		pr_err("MFRC522: could not find regmap\n");
		goto error_handle;
	}

	if (print_version(g_mfrc522) < 0)
		goto error_handle;

	pr_info("Hello, GISTRE card !\n");
	goto end;

error_handle:
	ret = -ENODEV;
	cdev_del(&g_mfrc522->cdev);
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