#include "debug.h"

void debug_log(enum type cmd, const char *buf)
{
	if (cmd == MEM_WRITE) {
		pr_info("<WR>\n");
	} else if (cmd == MEM_READ) {
		pr_info("<RD>\n");
	}

	for (size_t i = 0; i < MAX_CHAR_LINE; i++) {
		pr_info("%02x %02x %02x %02x %02x\n", buf[i * 5],
			buf[i * 5 + 1], buf[i * 5 + 2], buf[i * 5 + 3],
			buf[i * 5 + 4]);
	}
}