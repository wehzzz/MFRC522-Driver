#include "mfrc522_spi.h"

#define TRAME_BUFSIZE 2

int spi_write_byte(struct spi_device *spi, u8 reg, u8 val)
{
	u8 tx[TRAME_BUFSIZE];

	tx[0] = (u8)((reg << 1) & 0x7E);
	tx[1] = val;

	return spi_write(spi, tx, TRAME_BUFSIZE);
}

int spi_read_byte(struct spi_device *spi, u8 reg, u8 *val)
{
	u8 tx[2], rx[2];
	int ret;

	tx[0] = (u8)(((reg << 1) & 0x7E) | 0x80);
	tx[1] = 0x00;

	ret = spi_write_then_read(spi, tx, 1, rx, 1);
	if (ret < 0)
		return ret;

	*val = rx[0];
	return 0;
}