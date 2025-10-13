#include "i2c.h"

int i2c_write(struct i2c_client *client, char *buf, size_t len, u8 addr)
{
	char message[27];

	buffer[0] = addr;
	memmove(buffer + 1, buf, len + 1);

	return i2c_master_send(client, message, len + 1);
}

int i2c_read(struct i2c_client *client, char *buf, u8 addr)
{
	struct i2c_msg msgs[2] = { {
					   .addr = client->addr,
					   .flags = 0, // write
					   .len = 1,
					   .buf = &reg,
				   },
				   {
					   .addr = client->addr,
					   .flags = I2C_M_RD, // read
					   .len = 1,
					   .buf = &val,
				   } };

	return i2c_transfer(client->adapter, msgs, 2);
}
