#ifndef MFRC522_SPI_H
#define MFRC522_SPI_H

#include <linux/spi/spi.h>
#include <linux/string.h>

int spi_write_byte(struct spi_device *spi, u8 reg, const u8 val);
int spi_read_byte(struct spi_device *spi, u8 reg, u8 *val);

#endif /* ! MFRC522_SPI_H */