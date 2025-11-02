#ifndef COMMANDS_H
#define COMMANDS_H

#include "mfrc522_core.h"
#include "mfrc522_spi.h"

#define CMDARGS_SEP ":"
#define __MEM_WRITE "mem_write"
#define __MEM_READ "mem_read"
#define __GENERATE_RANDOM_ID "gen_rand_id"
#define __DEBUG "debug"
#define __DEBUG_ON "on"
#define __DEBUG_OFF "off"

enum type {
	UNKNOWN_CMD = 0,
	MEM_WRITE,
	MEM_READ,
	GENERATE_RANDOM_ID,
	DEBUG,
};

typedef int (*command)(struct mfrc522_dev *mfrc522, char *args);

int command_handle(struct mfrc522_dev *mfrc522, char *cmd);

#endif /* ! COMMANDS_H */