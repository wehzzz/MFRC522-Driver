PWD := $(shell pwd)
KVERSION := 6.12.51-v7+
KDIR := /lib/modules/$(KVERSION)/build
ccflags-y := -std=gnu11
obj-m += mfrc522.o
mfrc522-y := mfrc522_core.o mfrc522_utils.o mfrc522_commands.o mfrc522_spi.o

default: modules
modules:
	make -C "$(KDIR)" M="$(PWD)" modules

clean:
	make -C "$(KDIR)" M="$(PWD)" clean