
obj-m := hexcopydrv.o

#use this for building for ARM with KDIR set to source path
TOOLCHAIN=arm-linux-gnueabi-
arch=arm

KDIR := /lib/modules/$(shell uname -r)/build
#KDIR = /home/pintu/PINTU_HOME_WORK/PintuHomeTest/MY_PROJECT/RASPBERRY-PI/PI-2/linux
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean



