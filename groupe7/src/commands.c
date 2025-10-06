#include "commands.h"

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

int command_handle(char *cmd)
{
	enum type command_type;
	char *command = strsep(&cmd, ":");

	if (!command) {
		pr_err("MFRC522: Parse command: failed to extract command\n");
		return -EFAULT;
	}

	command_type = command_dispatch(command);
	if (command_type == UNKNOWN_CMD)
		return -EINVAL;

	return commands[command_type](cmd);
}

static int mem_write(char *args)
{
	pr_info("mem_write");
	return 0;
}
static int mem_read(char *args)
{
	pr_info("mem_read");
	return 0;
}
static int gen_rand_id(char *args)
{
	pr_info("gen_rand_id");
	return 0;
}