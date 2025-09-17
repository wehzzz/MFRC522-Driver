#include <linux/kernel.h>
#include <linux/module.h>

MODULE_AUTHOR("Anton VELLA <anton.vella@epita.fr>");
MODULE_AUTHOR("Martin LEVESQUE <martin.levesque@epita.fr>");
MODULE_DESCRIPTION("MFRC522 card reader driver");
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
