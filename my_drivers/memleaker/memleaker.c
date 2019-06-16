
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>	/* for kmalloc() and kfree() */
#include <linux/uaccess.h> /* for copy to/from users */
#include <linux/mman.h>	/* for mmap() operation */
#include <linux/io.h>
#include <linux/string.h>
#include <linux/ioctl.h>
#include <linux/gfp.h>
#include <linux/swap.h>
#include <linux/mmzone.h>


#define DEVICE_NAME	"memleaker"
#define MEMLEAKER_MAGIC	'k'
#define MEMLEAKER_ALLOC	_IOWR(MEMLEAKER_MAGIC, 1, int)
#define MEMLEAKER_FREE	_IOWR(MEMLEAKER_MAGIC, 2, int)

#define MB2PAGES(mb) ((mb) << (20 - PAGE_SHIFT))
#define MAX_PAGES	(1024)
static struct page *memleaker_page[MAX_PAGES];
/* split request in 1MB each = 2^8 order */
static int order = 8;

static unsigned long get_time()
{
	struct timeval value = {0,};
	unsigned long t;
	do_gettimeofday(&value);
	t = (value.tv_sec)*1000000 + (value.tv_usec);

	return t;
}

static ssize_t memleaker_read(struct file *file, char *buff,
				size_t length, loff_t *pos)
{
	pr_info("<MEMLEAKER> : Nothing to Read....\n");
	return 0;
}

static ssize_t memleaker_write(struct file *file, const char *buff,
				size_t length, loff_t *pos)
{
	int ret;
	int i;
	int nr_loop;
	/* split request in 1MB each = 2^8 order */
	//int order = 8;
	unsigned long bufsize = 0;
	unsigned int gfp_flags	= (GFP_HIGHUSER | __GFP_ZERO | __GFP_NOWARN |
				__GFP_NORETRY | __GFP_NOMEMALLOC |
				__GFP_NO_KSWAPD) & ~__GFP_WAIT;
	ret = kstrtoul_from_user(buff, length, 0, &bufsize);
	if (ret < 0) {
		pr_err("<MEMLEAKER>: Error: kstrtoul_from_user failed.\n");
		return -EINVAL;
	}
	if (MB2PAGES(bufsize) > nr_free_pages()) {
		pr_err("<MEMLEAKER>: Error: invalid bufsize(%lu)\n", bufsize);
		return -EINVAL;
	}
	/* bufsize should be in MB */
	nr_loop = bufsize;
	for (i = 0; i < nr_loop; i++) {
		memleaker_page[i] = alloc_pages(gfp_flags, order);
		if (!memleaker_page[i]) {
			int j;
			pr_err("MEMLEAKER: alloc_pages failed count:%d\n", i);
			for (j = 0; j < i; j++) {
				if (memleaker_page[j])
					__free_pages(memleaker_page[j], order);
			}
			return -ENOMEM;
		}
	}
	pr_info("<MEMLEAKER>: memleaker_write: bufsize: %lu MB, order: %d\n",
				bufsize, order);
	return length;
}

static long memleaker_ioctl(struct file *file, unsigned int cmd,
					unsigned long arg)
{
	return 0;
}

static int memleaker_mmap(struct file *file, struct vm_area_struct *vma)
{
	return 0;
}

static int memleaker_open(struct inode *inode, struct file *file)
{
	pr_info("<MEMLEAKER> : Device opened....\n");
	return 0;
}

static int memleaker_close(struct inode *inode, struct file *file)
{
	pr_info("<MEMLEAKER> : Device is Closed....\n");
	return 0;
}

static const struct file_operations memleaker_fops = {
	.owner = THIS_MODULE,
	.read  = memleaker_read,
	.write = memleaker_write,
	.unlocked_ioctl = memleaker_ioctl,
	.open  = memleaker_open,
	.mmap  = memleaker_mmap,
	.release = memleaker_close,
};

static struct miscdevice pin_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &memleaker_fops,
};

static int __init memleaker_init(void)
{
	int ret = 0;
	ret = misc_register(&pin_device);
	if (ret < 0) {
		pr_info("<MEMLEAKER> : Registration Failed....\n");
		return -EINVAL;
	}
	pr_info("<MEMLEAKER> : Device Registered Successfully...!\n");

	return 0;
}

static void __exit memleaker_exit(void)
{
	int i;
	pr_info("<MEMLEAKER> : Device exited..\n");
	for (i = 0; i < MAX_PAGES; i++) {
		if (memleaker_page[i])
			__free_pages(memleaker_page[i], order);
	}
	pr_info("<MEMLEAKER>: memleaker_exit: memleaker_pages freed\n");
	misc_deregister(&pin_device);
}

module_init(memleaker_init);
module_exit(memleaker_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PINTU KUMAR");
MODULE_DESCRIPTION("memleaker Driver...!");


