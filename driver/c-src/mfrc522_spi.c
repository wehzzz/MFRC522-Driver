#include "mfrc522_spi.h"

#define TRAME_BUFSIZE 2
#define READ_MODE 0x80
#define MASK 0x7E

int spi_write_byte(struct spi_device *spi, u8 reg, u8 val)
{
	u8 tx[TRAME_BUFSIZE];

	tx[0] = (u8)((reg << 1) & MASK);
	tx[1] = val;

	return spi_write(spi, tx, TRAME_BUFSIZE);
}

int spi_read_byte(struct spi_device *spi, u8 reg, u8 *val)
{
	u8 tx[1], rx[1];
	int ret;

	tx[0] = (u8)(((reg << 1) & MASK) | READ_MODE);

	ret = spi_write_then_read(spi, tx, 1, rx, 1);
	if (ret < 0)
		return ret;

	*val = rx[0];
	return 0;
}