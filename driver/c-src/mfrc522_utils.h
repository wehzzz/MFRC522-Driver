#ifndef MFRC522_UTILS_H
#define MFRC522_UTILS_H

#include "mfrc522_core.h"
#include "mfrc522_commands.h"

#define MAX_CHAR_LINE 5

int print_version(struct mfrc522_dev *mfrc522);
void debug_log(enum type cmd, const char *buf);
int reset_internal_memory(struct mfrc522_dev *mfrc522);

#endif /* ! MFRC522_UTILS_H */