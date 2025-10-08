PWD := $(shell pwd)
obj-m += gistre_card.o
gistre_card-y := entrypoint.o commands.o debug.o
ccflags-y := -std=gnu11

default: modules
modules clean:
	make -C "$(KDIR)" M="$(PWD)" "$@"
