#include "commands.h"

static int mem_write(struct card_dev *mfrc522, char *args)
{
	char *len;
	char *data;

	len = strsep(&args, ":");
	data = args;

	// TODO: Make this work I would like to check first if reg is writteable
	// if (!regmap_writeable(mfrc522->regmap, MFRC522_FIFODATAREG)) {
	// pr_err("MFRC522: Couldn't write: MFRC522_FIFODATAREG register is not writeable\n");
	// return -EPERM;
	// }

	pr_info("%s\n", len);
	pr_info("%s\n", data);
	pr_info("mem_write\n");
	return 0;
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
