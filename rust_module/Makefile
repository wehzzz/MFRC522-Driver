KVERSION := 6.12.51-v7+
KDIR := /lib/modules/$(KVERSION)/build

ccflags-y := -I$(src)

obj-m := rust_module.o

default: modules
modules:
	make -C "$(KDIR)" M="$(PWD)" modules

clean:
	make -C "$(KDIR)" M="$(PWD)" clean