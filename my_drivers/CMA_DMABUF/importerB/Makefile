
EXTRA_CFLAGS	+= -I$(shell pwd)
PWD := $(shell pwd)

obj-m := importerB.o
#pinchar-objs := pinchar_test.o

#KDIR := /lib/modules/$(shell uname -r)/
#KDIR := ?
CROSSCOMPILE=arm-none-linux-gnueabi-
ARCHITECTURE=arm

default:
	$(MAKE) ARCH=$(ARCHITECTURE) CROSS_COMPILE=$(CROSSCOMPILE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

