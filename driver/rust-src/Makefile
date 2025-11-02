KVERSION := 6.12.56-v8
KDIR := /lib/modules/$(KVERSION)/build
PWD := $(shell pwd)
ccflags-y := -std=gnu11
obj-m := mfrc522.o
mfrc522-y := mfrc522_core.o

default: modules
modules:
	make -B LLVM=1 -C "$(KDIR)" M="$(PWD)" modules

clean:
	make -B LLVM=1 -C "$(KDIR)" M="$(PWD)" clean
