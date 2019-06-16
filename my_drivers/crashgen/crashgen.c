
/*
 * crashgen.c (Low Memory Recovery)
 *
 * Copyright (C) 2011 Samsung India Software Operations.
 * Author: Pintu Kumar <pintu.k@samsung.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
*/

#include<linux/module.h>
#include<linux/errno.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/slab.h>
#include<linux/miscdevice.h>
#include<linux/uaccess.h>
#include<linux/mm.h>
#include<linux/mman.h>
#include<linux/io.h>
#include<linux/string.h>
#include<linux/ioctl.h>
#include<linux/swap.h>
#include<linux/delay.h>
#include "crashgen.h"


#define DEVICE_NAME	"crashgen"


static ssize_t crashgen_read(struct file *file, char *buff,
				size_t length, loff_t *pos)
{
	printk(KERN_INFO "[CRASHGEN]: Reading....\n");
	return 0;
}

static ssize_t crashgen_write(struct file *file, const char *buff,
				size_t length, loff_t *pos)
{
	int ret = -1;
	unsigned long len = 0;
	ret = kstrtoul_from_user(buff, length, 0, &len);
	if (ret < 0) {
		printk(KERN_ERR "[CRASHGEN]: kstrtoul_from_user: Failed !\n");
		return -1;
	}
	return 0;
}


static int crashgen_mmap(struct file *file, struct vm_area_struct *vma)
{
	printk(KERN_INFO "[CRASHGEN]: mmap called....\n");
	return 0;
}

static long crashgen_ioctl(struct file *file, unsigned int cmd,
				unsigned long arg)
{
	switch (cmd) {
		case CRASH_IOC_PTR_NONE:
		{
			int *ptr = NULL;
			pr_info("[CRASHGEN]: CRASH_IOC_PTR_NONE:\n");
			ptr++;
			*ptr = 1;
			break;
		}
		case CRASH_IOC_A:
		{
			break;
		}
		case CRASH_IOC_B:
		{
			break;
		}
		case CRASH_IOC_C:
		{
			break;
		}
		default:
			pr_info("[CRASHGEN]: default - NONE\n");
			break;
	}
	return 0;
}

static int crashgen_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "[CRASHGEN]: Device opened....\n");
	return 0;
}

static int crashgen_close(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "[CRASHGEN]: Device is Closed....\n");
	return 0;
}


static const struct file_operations crashgen_fops = {
	.owner		= THIS_MODULE,
	.read		= crashgen_read,
	.write		= crashgen_write,
	.unlocked_ioctl	= crashgen_ioctl,
	.open		= crashgen_open,
	.mmap		= crashgen_mmap,
	.release	= crashgen_close,
};

static struct miscdevice crashdevice = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= DEVICE_NAME,
	.fops	= &crashgen_fops,
};


static int __init crashgen_init(void)
{
	int ret = -1;
	printk(KERN_INFO "[CRASHGEN]: Device Initialized......\n");
	ret = misc_register(&crashdevice);
	if (ret < 0) {
		printk(KERN_ERR "[CRASHGEN]: Registration Failed....\n");
		return -1;
	}
	printk(KERN_INFO "[CRASHGEN]: Device Registered Successfully...!\n");
	return 0;
}

static void __exit crashgen_exit(void)
{
	printk(KERN_INFO "[CRASHGEN] : Device exited..\n");
	misc_deregister(&crashdevice);
}


module_init(crashgen_init);
module_exit(crashgen_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PINTU KUMAR");
MODULE_DESCRIPTION("crashgen Driver...!");

