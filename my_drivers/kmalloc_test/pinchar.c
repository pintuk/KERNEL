
#include<linux/module.h>
#include<linux/errno.h>
#include<linux/kernel.h>
#include <linux/miscdevice.h>
#include<linux/fs.h>
#include<linux/slab.h>			/* for kmalloc() and kfree() */
#include<asm/uaccess.h>			/* for copy to/from users */
#include<linux/mman.h>			/* for mmap() operation */
#include<asm/io.h>
#include<linux/string.h>
#include<linux/ioctl.h>
#include<linux/gfp.h>


#define DEVICE_NAME	"pinchar"
#define PINCHAR_MAGIC	'k'
#define PINCHAR_ALLOC	_IOWR(PINCHAR_MAGIC, 1, int)
#define PINCHAR_FREE	_IOWR(PINCHAR_MAGIC, 2, int)

#define MAX_BLOCK	(4096)

static int index = 0;
static struct page *pinchar_buffer;
static unsigned int order = 0;

static unsigned long get_time()
{
	struct timeval value = {0,};
	unsigned long t;
	do_gettimeofday(&value);
	t = (value.tv_sec)*1000000 + (value.tv_usec);

	return t;
}

static ssize_t pinchar_read(struct file *file, char *buff, 
				size_t length, loff_t *pos)
{
	pr_info("<PINCHAR> : Nothing to Read....\n");
	return 0;
}

static ssize_t pinchar_write(struct file *file, const char *buff, 
				size_t length, loff_t *pos)
{
	int ret;
	unsigned long bufsize = 0;
	//unsigned int order = 0;
	unsigned long t1,t2;
	unsigned int gfp_flags  = (GFP_HIGHUSER | __GFP_ZERO |
					 __GFP_NOWARN);
	ret = kstrtoul_from_user(buff, length, 0, &bufsize);
	if (ret < 0) {
		pr_info("<PINCHAR> : kstrtoul_from_user Failed....\n");
		return -EINVAL;
	}
	order = get_order(bufsize);
	pr_info("<PINCHAR>: bufsize: %lu, Order = %d\n",(unsigned long)bufsize,order);
	t1 = get_time();
	//pinchar_buffer = kzalloc(bufsize, GFP_KERNEL);
	pinchar_buffer = alloc_pages(gfp_flags, order);
	t2 = get_time();
	pr_info("<PINCHAR>:alloc_pages: time taken: %lu us\n", (t2-t1)); 
	if(pinchar_buffer == NULL) {
		pr_info("<PINCHAR> : alloc_pages failed\n");
		return -ENOMEM;
	}
	return length;
}

static long pinchar_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	return 0;
}

static int pinchar_mmap(struct file *file, struct vm_area_struct *vma)
{
	return 0;
}

static int pinchar_open(struct inode *inode, struct file *file)
{
	pr_info("<PINCHAR> : Device opened....\n");
	index = 0;
	return 0;
}

static int pinchar_close(struct inode *inode, struct file *file)
{
	pr_info("<PINCHAR> : Device is Closed....\n");
	index = 0;
	if (pinchar_buffer) {
		//kfree(pinchar_buffer);
		__free_pages(pinchar_buffer, order);
		pinchar_buffer = NULL;
	}
	return 0;
}


struct file_operations pinchar_fops = {
    .owner = THIS_MODULE,
    .read  = pinchar_read,
    .write = pinchar_write,
    .unlocked_ioctl = pinchar_ioctl,
    .open  = pinchar_open,
    .mmap  = pinchar_mmap,
    .release = pinchar_close,
};

static struct miscdevice pin_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &pinchar_fops,
};



static int __init pinchar_init(void)
{
    int ret = 0;
    ret = misc_register(&pin_device);
    if(ret < 0)
    {
	pr_info("<PINCHAR> : Registration Failed....\n");
	return -1;
    }
    pr_info("<PINCHAR> : Device Registered Successfully...!\n");

    return 0;
}

static void __exit pinchar_exit(void)
{
    pr_info("<PINCHAR> : Device exited..\n");
    misc_deregister(&pin_device);
}


module_init(pinchar_init);
module_exit(pinchar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PINTU KUMAR");
MODULE_DESCRIPTION("pinchar Driver...!");


