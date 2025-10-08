#include "commands.h"
#include "debug.h"

static int debug(struct card_dev *mfrc522, char *args)
{
	char *debug_mode;

	debug_mode = strsep(&args, ":");
	if (!debug_mode) {
		pr_err("MFRC522: Parse command: failed to extract debug_mode\n");
		return -EINVAL;
	}

	if (strcmp(debug_mode, "on") == 0) {
		mfrc522->debug = true;
	} else if (strcmp(debug_mode, "off") == 0) {
		mfrc522->debug = false;
	} else {
		pr_err("MFRC522: Parse command: undefined debug_mode\n");
		return -EINVAL;
	}
	return 0;
}

static int mem_write(struct card_dev *mfrc522, char *args)
{
	char *len_arg;
	char *data;
	unsigned len;
	int ret;
	int i = 0;
	char debug_str[MFRC522_BUFSIZE];

	len_arg = strsep(&args, ":");
	if (!len_arg) {
		pr_err("MFRC522: Parse command: failed to extract length\n");
		return -EINVAL;
	}
	if (kstrtou32(len_arg, 10, &len) != 0) {
		pr_err("MFRC522: Parse command: failed to convert length to unsigned\n");
		return -EINVAL;
	}
	len = (len > MFRC522_BUFSIZE) ? MFRC522_BUFSIZE : len;
	data = args;

	for (; i < len; i++) {
		if ((ret = regmap_write(mfrc522->regmap, MFRC522_FIFODATAREG,
					data[i])) < 0) {
			pr_err("MFRC522: failed to write data to FIFO\n");
			return ret;
		}
		debug_str[i] = data[i];
	}

	for (; i < MFRC522_BUFSIZE; i++) {
		if ((ret = regmap_write(mfrc522->regmap, MFRC522_FIFODATAREG,
					'\0')) < 0) {
			pr_err("MFRC522: failed to write data to FIFO\n");
			return ret;
		}
		debug_str[i] = '\0';
	}

	if ((ret = regmap_write(mfrc522->regmap, MFRC522_CMDREG, MFRC522_MEM)) <
	    0) {
		pr_err("MFRC522: failed to write FIFO to internal memory\n");
		return ret;
	}

	if (mfrc522->debug)
		debug_log(MEM_WRITE, debug_str);

	return len;
}

static int mem_read(struct card_dev *mfrc522, char *args)
{
	int ret;
	int value;

	if ((ret = regmap_write(mfrc522->regmap, MFRC522_FIFOLEVELREG,
				MFRC522_FIFOLEVELREG_FLUSH)) < 0) {
		pr_err("MFRC522: failed to flush FIFO\n");
		return ret;
	}

	if ((ret = regmap_write(mfrc522->regmap, MFRC522_CMDREG, MFRC522_MEM)) <
	    0) {
		pr_err("MFRC522: failed to write internal memory to FIFO\n");
		return ret;
	}

	for (int i = 0; i < MFRC522_BUFSIZE; i++) {
		if ((ret = regmap_read(mfrc522->regmap, MFRC522_FIFODATAREG,
				       &value)) < 0) {
			pr_err("MFRC522: failed to read data from FIFO\n");
			return ret;
		}
		mfrc522->buf[i] = value;
	}

	if (mfrc522->debug)
		debug_log(MEM_READ, mfrc522->buf);

	return MFRC522_BUFSIZE;
}

static int gen_rand_id(struct card_dev *mfrc522, char *args)
{
	int ret;

	if ((ret = regmap_write(mfrc522->regmap, MFRC522_CMDREG,
				MFRC522_GENERATERANDOMID)) < 0) {
		pr_err("MFRC522: failed to write GENERATERANDOMID command to register\n");
		return ret;
	}

	return MFRC522_BUFSIZE;
}

static const command commands[] = {
	[MEM_WRITE] = mem_write,
	[MEM_READ] = mem_read,
	[GENERATE_RANDOM_ID] = gen_rand_id,
	[DEBUG] = debug,
};

static enum type command_dispatch(char *cmd)
{
	if (strcmp(cmd, "mem_write") == 0)
		return MEM_WRITE;
	else if (strcmp(cmd, "mem_read") == 0)
		return MEM_READ;
	else if (strcmp(cmd, "gen_rand_id") == 0)
		return GENERATE_RANDOM_ID;
	else if (strcmp(cmd, "debug") == 0)
		return DEBUG;
	return UNKNOWN_CMD;
}

int command_handle(struct card_dev *mfrc522, char *cmd)
{
	enum type command_type;
	char *command = strsep(&cmd, ":");

	if (!command) {
		pr_err("MFRC522: Parse command: failed to extract command\n");
		return -EFAULT;
	}

	command_type = command_dispatch(command);
	if (command_type == UNKNOWN_CMD) {
		pr_err("MFRC522: Parse command: unrecognised command\n");
		return -EINVAL;
	}

	return commands[command_type](mfrc522, cmd);
}
