
EXTRA_CFLAGS	+= -I$(shell pwd)

obj-m := pinchar.o
#pinchar-objs := pinchar_test.o

#KDIR := /lib/modules/$(shell uname -r)/
#KDIR := /lib/modules/2.6.32-30-generic-pae/build
#KDIR := /root/Aquila-Kernel/linux-2.6.29-mobile/
#KDIR := /root/PINTU_UBUNTU/SISO_WORK/HQ_GIT/linux-2.6.32/
#KDIR := /root/PINTU_UBUNTU/SISO_WORK/LINUX_MM/TEST/OMAP4_KERNEL_TEST/linux-slp-omap4-kernel2.6.36/
#KDIR := /home/pintu/HQ_GIT/linux-2.6.36/
#KDIR := /home/pintu/HQ_GIT/Pintu_U1_Kernel36/linux-2.6.36/
#KDIR := /home/pintu/HQ_GIT//Memory_Frag_Work/MEM_COMPACTION_22NOV2011/linux-2.6.36/
#KDIR := /home/pintu/HQ_GIT/WORK_2012/IOMMU_WORK/proxima_kernel_15Mar2012/
KDIR := /home/pintu/SISO_WORK/2015_WORK/SPRD/autolaunch_test/linux-3.10-sc7727/
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

