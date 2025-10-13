#include "mfrc522_i2c.h"

#define BYTE_SIZE 1
#define REG_SIZE 1

int i2c_write_byte(struct i2c_client *client, u8 reg, u8 val)
{
	int ret;

	ret = i2c_smbus_write_byte_data(client, reg, val);
	if (ret < 0)
		pr_err("MFRC522: i2c_smbus_write_byte_data failed addr=0x%02x reg=0x%02x val=0x%02x ret=%d\n",
		       client->addr, reg, val, ret);
	else
		pr_debug("MFRC522: wrote reg=0x%02x val=0x%02x\n", reg, val);

	return ret;
}

int i2c_read_byte(struct i2c_client *client, u8 reg, u8 *val)
{
	int ret;

	ret = i2c_smbus_read_byte_data(client, reg);
	if (ret < 0) {
		pr_err("MFRC522: i2c_smbus_read_byte_data failed reg=0x%02x ret=%d\n",
		       reg, ret);
		return ret;
	}

	*val = (u8)ret;
	pr_debug("MFRC522: read reg=0x%02x val=0x%02x\n", reg, *val);
	return 0;
}