#include "mfrc522_core.h"
#include "mfrc522_commands.h"

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Anton VELLA <anton.vella@epita.fr>");
MODULE_AUTHOR("Martin LEVESQUE <martin.levesque@epita.fr>");
MODULE_DESCRIPTION("MFRC522 card reader driver");

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
static struct mfrc522_dev *g_mfrc522;
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
	size_t buf_len;
	struct mfrc522_dev *mfrc522;

	(void)off;

	mfrc522 = (struct mfrc522_dev *)file->private_data;
	buf_len = strlen(mfrc522->buf);

	if (len < buf_len) {
		pr_err("MFRC522: When copying data to user, did not find enough buffer space\n");
		return -ENOSPC;
	}

	len = min(len, buf_len);

	if (copy_to_user(buf, mfrc522->buf, len)) {
		pr_err("MFRC522: When copying data to user, failed memory copy verification\n");
		return -EFAULT;
	}
	memset(mfrc522->buf, 0, MFRC522_BUFSIZE + 1);

	return len;
}

static ssize_t mfrc522_write(struct file *file, const char __user *buf,
			     size_t len, loff_t *off)
{
	int ret;
	struct mfrc522_dev *mfrc522;
	char *kbuf;

	(void)off;

	mfrc522 = (struct mfrc522_dev *)file->private_data;
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

static int mfrc522_probe(struct i2c_client *client)
{
	dev_t dev;
	int ret = 0;

	dev_info(&client->dev, "Probing MFRC522 rfid card at 0x%02x\n",
		 client->addr);

	/* Step 1: dynamically allocate a major number */
	ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
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
	g_mfrc522->debug = 0;
	memset(g_mfrc522->buf, 0, MFRC522_BUFSIZE);

	ret = cdev_add(&g_mfrc522->cdev, dev, 1);
	if (ret < 0) {
		pr_err("MFRC522: failed to add device to kernel\n");
		goto free_dev;
	}

	g_mfrc522->dev = &client->dev;
	g_mfrc522->client = client;

	if (print_version(g_mfrc522) < 0)
		goto error_handle;

	pr_info("Hello, GISTRE card !\n");
	goto end;

error_handle:
	ret = -ENODEV;
free_dev:
	kfree(g_mfrc522);
unregister_dev:
	unregister_chrdev_region(dev, 1);
end:
	return ret;
}

static void mfrc522_remove(struct i2c_client *client)
{
	dev_t dev;
	dev_info(&client->dev, "Removing MFRC522 rfid card driver\n");

	cdev_del(&g_mfrc522->cdev);

	dev = MKDEV(g_major, 0);
	kfree(g_mfrc522);

	unregister_chrdev_region(dev, 1);
	pr_info("Goodbye, GISTRE card !\n");
}

static const struct of_device_id mfrc522_dt_id[] = { { .compatible =
							       "nxp,mfrc522" },
						     {} };
MODULE_DEVICE_TABLE(of, mfrc522_dt_id);

static const struct i2c_device_id mfrc522_id[] = { { "mfrc522", 0 }, {} };
MODULE_DEVICE_TABLE(i2c, mfrc522_id);

static struct i2c_driver mfrc522_driver = {
    .driver = {
        .name = DEVICE_NAME,
        .of_match_table = mfrc522_dt_id,
    },
    .probe = mfrc522_probe,
    .remove = mfrc522_remove,
    .id_table = mfrc522_id,
};

module_i2c_driver(mfrc522_driver);