PWD := $(shell pwd)
KVERSION := 6.12.51-v7+
KDIR := /lib/modules/$(KVERSION)/build
ccflags-y := -std=gnu11
obj-m += mfrc522.o
mfrc522-y := mfrc522_core.o mfrc522_debug.o mfrc522_commands.o

default: modules
modules clean:
	make -C "$(KDIR)" M="$(PWD)" "$@"
