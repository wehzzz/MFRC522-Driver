#include "commands.h"

enum type command_dispatch(char *cmd)
{
	if (strcmp(cmd, "mem_write") == 0)
		return MEM_WRITE;
	if (strcmp(cmd, "mem_read") == 0)
		return MEM_READ;
	if (strcmp(cmd, "gen_rand_id") == 0)
		return GENERATE_RANDOM_ID;
	return -EINVAL;
}

int command_handler(char *cmd)
{
	enum type command_type = command_dispatch(cmd);
	commands[command_type]();
	return 0;
}

static int mem_write(void)
{
	return 0;
}
static int mem_read(void)
{
	return 0;
}
static int gen_rand_id(void)
{
	return 0;
}