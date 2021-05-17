
obj-m := xarray-test.o

ARCH ?=
CROSS_COMPILE ?=
# KDIR := /lib/modules/$(shell uname -r)/build
KDIR ?= 
PWD := $(shell pwd)

default:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) modules
	#$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean



