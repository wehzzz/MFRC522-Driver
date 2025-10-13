#include "mfrc522_core.h"
#include "mfrc522_commands.h"

#define MAX_CHAR_LINE 5

void debug_log(enum type cmd, const char *buf);