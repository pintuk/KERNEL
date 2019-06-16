
EXTRA_CFLAGS	+= -I$(shell pwd)
PWD := $(shell pwd)

obj-m := exporterA.o
#pinchar-objs := pinchar_test.o

#KDIR := /lib/modules/$(shell uname -r)/
#KDIR := /home/pintu/HQ_GIT/WORK_2012/IOMMU_WORK/proxima_kernel_15Mar2012/
#KDIR := /home/pintu/HQ_GIT/WORK_2012/IOMMU_WORK/30April2012/SISO/linux-2.6/
KDIR := /home/pintu/HQ_GIT/WORK_2012/IOMMU_WORK/30April2012/May17/linux-2.6/
CROSSCOMPILE=/scratchbox/compilers/arm-linux-gnueabi-gcc4.4.1-glibc2.11.1/bin/arm-none-linux-gnueabi-
ARCHITECTURE=arm

default:
	$(MAKE) ARCH=$(ARCHITECTURE) CROSS_COMPILE=$(CROSSCOMPILE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

