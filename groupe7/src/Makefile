KDIR := /usr/src/linux-headers-$(shell uname -r)
PWD  := $(shell pwd)

obj-m := hello-world.o

default: modules
modules clean:
	make -C "$(KDIR)" M="$(PWD)" "$@"
