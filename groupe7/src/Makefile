PWD  := $(shell pwd)

obj-m := gistre_card.o

default: modules
modules clean:
	make -C "$(KDIR)" M="$(PWD)" "$@"
