EXTRA_CFLAGS	+= -I$(shell pwd)

#EXTRA_LDFLAGS	+= -lm

obj-m := crashgen.o

TOOLCHAIN=arm-linux-gnueabi-

#KDIR := /lib/modules/$(shell uname -r)/build
#KDIR:= /home/pintu/HQ_GIT/WORK_2012/SLP_PROPOSAL/FullMemoryReclaim/JULY_KERNEL/linux-2.6/
#KDIR:= /home/pintu/SISO_WORK/HQ_GIT/SLP_WORK/WORK_2013/KERNEL_CODE/REDWOOD/slp-kernel-3.4/08Jan2013/linux/
#KDIR:= /home/pintu/SISO_WORK/HQ_GIT/SLP_WORK/WORK_2013/KERNEL_CODE/Redwood-8974/git-code/qc-linux
#KDIR:= /home/pintu/SISO_WORK/HQ_GIT/SLP_WORK/2014WORK/SPRD_WORK/KERNEL_CODE/GitCode/sprd-linux
KDIR = /home/pintu/SISO_WORK/2015_WORK/SPRD/Z3-ALL/linux-3.10-sc7730
PWD := $(shell pwd)

default:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(TOOLCHAIN) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean



