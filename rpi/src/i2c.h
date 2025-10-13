#ifndef I2C_H
#define I2C_H

#include <linux/i2c.h>
#include <linux/string.h>

#define SLAVE_ADDR TODO

struct i2c_client *i2c_init();
int i2c_write(struct i2c_client * client, char *buf, unsigned addr);
int i2c_read(struct i2c_client * client, char *buf, unsigned addr);

#endif /* ! I2C_H */