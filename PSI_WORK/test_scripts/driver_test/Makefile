
obj-m := psi_kernel_test.o

#ARCH ?=
#CROSS_COMPILE ?= arm-linux-gnueabi-
KDIR ?= /lib/modules/$(uname -r)/build
PWD := $(shell pwd)

default:
	#$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) modules
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

