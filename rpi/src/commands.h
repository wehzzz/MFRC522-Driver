#ifndef COMMANDS_H
#define COMMANDS_H

#include "entrypoint.h"

enum type {
	UNKNOWN_CMD = 0,
	MEM_WRITE,
	MEM_READ,
	GENERATE_RANDOM_ID,
	DEBUG,
};

typedef int (*command)(struct card_dev *mfrc522, char *args);

int command_handle(struct card_dev *mfrc522, char *cmd);

#endif /* ! COMMANDS_H */