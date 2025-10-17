PWD := $(shell pwd)
obj-m += gistre_card.o
gistre_card-y := mfrc522_core.o mfrc522_commands.o mfrc522_utils.o
ccflags-y := -std=gnu11

default: modules
modules:
	make -I"$(CPATH)" -C "$(KDIR)" M="$(PWD)" modules

clean:
	make -I"$(CPATH)" -C "$(KDIR)" M="$(PWD)" clean
