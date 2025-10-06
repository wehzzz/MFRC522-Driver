#include "commands.h"

static int mem_write(struct card_dev *mfrc522, char *args)
{
	char *len_arg;
	char *data;
	unsigned len;
	int ret;

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

	memcpy(mfrc522->buf, data, len);
	memset(mfrc522->buf + len, 0, MFRC522_BUFSIZE - len);

	for (int i = 0; i < MFRC522_BUFSIZE; i++) {
		if ((ret = regmap_write(mfrc522->regmap, MFRC522_FIFODATAREG,
					mfrc522->buf[i])) < 0) {
			pr_err("MFRC522: failed to write data to FIFO\n");
			return ret;
		}
	}

	return len;
}

static int mem_read(struct card_dev *mfrc522, char *args)
{
	pr_info("mem_read\n");
	return 0;
}

static int gen_rand_id(struct card_dev *mfrc522, char *args)
{
	pr_info("gen_rand_id\n");
	return 0;
}

static const command commands[] = {
	[MEM_WRITE] = mem_write,
	[MEM_READ] = mem_read,
	[GENERATE_RANDOM_ID] = gen_rand_id,
};

enum type command_dispatch(char *cmd)
{
	if (strcmp(cmd, "mem_write") == 0)
		return MEM_WRITE;
	else if (strcmp(cmd, "mem_read") == 0)
		return MEM_READ;
	else if (strcmp(cmd, "gen_rand_id") == 0)
		return GENERATE_RANDOM_ID;
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
