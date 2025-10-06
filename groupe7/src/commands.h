#ifndef COMMANDS_H
#define COMMANDS_H

#include "entrypoint.h"

enum type {
	MEM_WRITE,
	MEM_READ,
	GENERATE_RANDOM_ID,
};

static int mem_write(void);
static int mem_read(void);
static int gen_rand_id(void);

typedef int (*command)(void);

static const command commands[] = {
	[MEM_WRITE] = mem_write,
	[MEM_READ] = mem_read,
	[GENERATE_RANDOM_ID] = gen_rand_id,
};

int command_handle(char *cmd);

#endif /* ! COMMANDS_H */