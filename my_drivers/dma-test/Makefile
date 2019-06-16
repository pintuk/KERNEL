
EXTRA_CFLAGS	+= -I$(shell pwd)
PWD := $(shell pwd)

obj-m := dmabuftest.o

KDIR := /home/pintu/SISO_WORK/2015_WORK/SPRD/Z3-ALL/linux-3.10-sc7730/
CROSSCOMPILE=/opt/toolchains/tizen2.4_cross_toolchain_for_32bit_host/bin/armv7l-tizen-linux-gnueabi-
ARCHITECTURE=arm

default:
	$(MAKE) ARCH=$(ARCHITECTURE) CROSS_COMPILE=$(CROSSCOMPILE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

