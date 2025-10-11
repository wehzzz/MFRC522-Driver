#include "mfrc522_core.h"

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Anton VELLA <anton.vella@epita.fr>");
MODULE_AUTHOR("Martin LEVESQUE <martin.levesque@epita.fr>");
MODULE_DESCRIPTION("MFRC522 card reader driver");

static int mfrc522_probe(struct i2c_client *client)
{
	dev_info(&client->dev, "Probing MFRC522 rfid card at 0x%02x\n",
		 client->addr);
	return 0;
}

static void mfrc522_remove(struct i2c_client *client)
{
	dev_info(&client->dev, "Removing MFRC522 rfid card driver\n");
}

static const struct of_device_id mfrc522_dt_id[] = { { .compatible =
							       "npx,mfrc522" },
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