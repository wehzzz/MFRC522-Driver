PWD  := $(shell pwd)
obj-m += gistre_card.o
gistre_card-y := commands.o entrypoint.o

default: modules
modules clean:
	make -I"$(CPATH)" -C "$(KDIR)" M="$(PWD)" "$@"
