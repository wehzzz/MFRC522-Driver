#ifndef COMMANDS_H
#define COMMANDS_H

#include "mfrc522_core.h"

enum type {
	UNKNOWN_CMD = 0,
	MEM_WRITE,
	MEM_READ,
	GENERATE_RANDOM_ID,
	DEBUG,
};

typedef int (*command)(struct mfrc522_dev *mfrc522, char *args);

int command_handle(struct mfrc522_dev *mfrc522, char *cmd);
int print_version(struct mfrc522_dev *mfrc522);

#endif /* ! COMMANDS_H */