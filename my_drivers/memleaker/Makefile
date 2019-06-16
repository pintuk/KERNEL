
EXTRA_CFLAGS	+= -I$(shell pwd)

obj-m := memleaker.o

KDIR := /home/pintu/SISO_WORK/2015_WORK/SPRD/Z3-ALL/linux-3.10-sc7730/
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

