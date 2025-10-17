#include "mfrc522_utils.h"

int print_version(struct card_dev *mfrc522)
{
	int ret;
	unsigned int version;

	if ((ret = of_property_read_u32(mfrc522->dev->of_node,
					MFRC522_PROP_VERSION, &version)) < 0) {
		pr_err("MFRC522: could not retrieve version property\n");
		return ret;
	}

	pr_info("MFRC522: version v%u\n", version);
	return 0;
}

void debug_log(enum type cmd, const char *buf)
{
	if (cmd == MEM_WRITE) {
		pr_info("<WR>\n");
	} else if (cmd == MEM_READ) {
		pr_info("<RD>\n");
	}

	for (size_t i = 0; i < MAX_CHAR_LINE; i++) {
		pr_info("%02x %02x %02x %02x %02x\n", buf[i * 5],
			buf[i * 5 + 1], buf[i * 5 + 2], buf[i * 5 + 3],
			buf[i * 5 + 4]);
	}
}

int reset_internal_memory(struct card_dev *mfrc522)
{
	int ret;
	for (int i = 0; i < MFRC522_BUFSIZE; i++) {
		if ((ret = regmap_write(mfrc522->regmap, MFRC522_FIFODATAREG,
					'\0')) < 0) {
			pr_err("MFRC522: failed to write data to FIFO\n");
			return ret;
		}
	}

	if ((ret = regmap_write(mfrc522->regmap, MFRC522_CMDREG, MFRC522_MEM)) <
	    0) {
		pr_err("MFRC522: failed to write internal memory to FIFO\n");
		return ret;
	}
	return 0;
}