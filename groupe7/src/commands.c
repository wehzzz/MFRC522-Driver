#include "commands.h"

enum type command_dispatch(char *cmd)
{
	char *command = strsep(&cmd, ":");

	if (!command) {
		pr_err("MFRC522: Parse command: failed to extract command\n");
		return -EFAULT;
	}

	if (strcmp(command, "mem_write") == 0)
		return MEM_WRITE;
	else if (strcmp(command, "mem_read") == 0)
		return MEM_READ;
	else if (strcmp(command, "gen_rand_id") == 0)
		return GENERATE_RANDOM_ID;
	return -EINVAL;
}

int command_handle(char *cmd)
{
	enum type command_type = command_dispatch(cmd);
	commands[command_type]();
	return 0;
}

static int mem_write(void)
{
	pr_info("mem_write");
	return 0;
}
static int mem_read(void)
{
	pr_info("mem_read");
	return 0;
}
static int gen_rand_id(void)
{
	pr_info("gen_rand_id");
	return 0;
}