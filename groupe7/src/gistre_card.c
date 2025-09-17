#include <linux/kernel.h>
#include <linux/module.h>

MODULE_AUTHOR("Anton et Martin");
MODULE_DESCRIPTION("module for gistre_card");
MODULE_LICENSE("GPL v2");

__init
static int gistre_card_init(void) {
	pr_info("Hello, GISTRE card !\n");
	return 0;
}

__exit
static void gistre_card_exit(void) {
}

module_init(gistre_card_init);
module_exit(gistre_card_exit);
