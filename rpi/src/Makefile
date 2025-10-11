PWD := $(shell pwd)
obj-m += test.o
ccflags-y := -std=gnu11
KDIR := /lib/modules/6.12.51-v7+/build

default: modules
modules clean:
	make -C "$(KDIR)" M="$(PWD)" "$@"
