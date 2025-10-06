#ifndef COMMANDS_H
#define COMMANDS_H

#include "entrypoint.h"

enum type {
	UNKNOWN_CMD = 0,
	MEM_WRITE,
	MEM_READ,
	GENERATE_RANDOM_ID,
};

static int mem_write(char *args);
static int mem_read(char *args);
static int gen_rand_id(char *args);

typedef int (*command)(char *args);

static const command commands[] = {
	[MEM_WRITE] = mem_write,
	[MEM_READ] = mem_read,
	[GENERATE_RANDOM_ID] = gen_rand_id,
};

int command_handle(char *cmd);

#endif /* ! COMMANDS_H */