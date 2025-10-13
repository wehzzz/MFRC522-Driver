#ifndef I2C_H
#define I2C_H

#include <linux/i2c.h>
#include <linux/string.h>

#define SLAVE_ADDR TODO

int i2c_write_byte(struct i2c_client *client, u8 reg, const u8 val);
int i2c_read_byte(struct i2c_client *client, u8 reg, u8 *val);

#endif /* ! I2C_H */