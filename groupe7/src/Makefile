PWD  := $(shell pwd)
obj-m := gistre_card.o

default: modules
modules clean:
	make -I"$(CPATH)" -C "$(KDIR)" M="$(PWD)" "$@"
