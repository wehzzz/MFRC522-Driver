KVERSION := 6.12.56-v8+
KDIR := /lib/modules/$(KVERSION)/build
PWD := $(shell pwd)

obj-m := mfrc522.o
mfrc522-y := rust_module.o

default: modules
modules:
	make -B LLVM=1 -C "$(KDIR)" M="$(PWD)" modules

clean:
	make -B LLVM=1 -C "$(KDIR)" M="$(PWD)" clean
