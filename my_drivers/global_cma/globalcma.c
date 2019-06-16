
#include<linux/module.h>
#include<linux/errno.h>
#include<linux/kernel.h>
#include <linux/miscdevice.h>
#include<linux/fs.h>
#include<linux/slab.h>			/* for kmalloc() and kfree() */
#include<asm/uaccess.h> 		/* for copy to/from users */
#include<linux/mman.h>			/* for mmap() operation */
#include<asm/io.h>
#include<linux/string.h>
#include<linux/ioctl.h>
#include<linux/gfp.h>

#include <linux/dma-mapping.h>
#include <linux/bootmem.h>
#include <linux/dma-contiguous.h>
#include <asm/page.h>
#include <asm/sizes.h>



#define DEVICE_NAME	"pinchar"
#define PINCHAR_MAGIC   'k'
#define PINCHAR_ALLOC	_IOWR(PINCHAR_MAGIC, 1, int)
#define PINCHAR_FREE	_IOWR(PINCHAR_MAGIC, 2, int)
//#define PINCHAR_ALLOC	1  //_IOWR(PINCHAR_MAGIC, 1, int)
//#define PINCHAR_FREE	2  //_IOWR(PINCHAR_MAGIC, 2, int)

#define MAX_BLOCK	(4096)

//static int major_no=-1;
static int index = 0;
static char *pinchar_buffer[MAX_BLOCK+1];
int order = 0;

unsigned long cma_size = 0;


void *cma_alloc(int size)
{
     unsigned int count = PAGE_ALIGN(size) >> PAGE_SHIFT;
     struct page *page = dma_alloc_from_contiguous(NULL, count, 0);
     if (!IS_ERR(page))
         return page_address(page);
     return page;
}

void cma_free(void *ptr, int size)
{
     struct page *page = virt_to_page(ptr);
     unsigned int count = PAGE_ALIGN(size) >> PAGE_SHIFT;
     dma_release_from_contiguous(NULL, page, count);
}


static ssize_t pinchar_read(struct file *file, char *buff, size_t length, loff_t *pos)
{
	printk("<PINCHAR> : Reading....\n");
	if(pinchar_buffer[0] == NULL) 
	{
		printk("<PINCHAR> : Buffer Empty...\n");
		return -ENOMEM;
	}
	if(copy_to_user(buff,pinchar_buffer[0],length)) 
	{
		return -EFAULT;
	}
	return(length);
}

static ssize_t pinchar_write(struct file *file, const char *buff, size_t length, loff_t *pos)
{
	printk("<PINCHAR> : Writing....len = %d\n",length);
	if(pinchar_buffer[0] == NULL)
	{
		printk("<PINCHAR> : buffer not allocated ....\n");
		return -EFAULT;
	}
	if(copy_from_user(pinchar_buffer[0],buff,length)) 
	{
		return -EFAULT;
	}
	printk("<PINCHAR> : pinchar_buffer = %s\n",pinchar_buffer[0]);
    	return(length);
}

//static int pinchar_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
static long pinchar_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
		case PINCHAR_ALLOC:
		{
			unsigned long int len = arg;
			if(index > MAX_BLOCK)
			{
				printk("<PINCHAR> : Warning - PINCHAR_ALLOCATE : index out of range(4096)\n");
				return -1;
			}
			//order = get_order(len);
			//if(order < 0) return (-1);
			//pagebuffer[index] = alloc_pages(GFP_HIGHUSER, order);
			
			//pinchar_buffer[index] = kmalloc(sizeof(char)*len,GFP_KERNEL);
			cma_size = len;
			pinchar_buffer[index] = cma_alloc(cma_size);
			if(pinchar_buffer[index] == NULL)
			{
				printk("<PINCHAR> : ERROR - PINCHAR_ALLOCATE : cannot allocate memory of %ld\n",len);
				return -1;
			}
			memset(pinchar_buffer[index],0,sizeof(char)*(len));
			
			printk("<PINCHAR> : PINCHAR_ALLOCATE - Success(index = %d) !\n",index);
			index++;
		}
		break;
		
		case PINCHAR_FREE:
		{
			int i = arg;

			if(pinchar_buffer[i] != NULL)
			{
				//kfree(pinchar_buffer[i]);
				cma_free(pinchar_buffer[i], cma_size);
				//__free_pages(pagebuffer[i], order);
				pinchar_buffer[i]=NULL;
				printk("<PINCHAR> : PINCHAR_FREE [block = %d] - DONE !\n",i);
			}
			
			//index = 0;
		}
		break;
		
		default:
			return -EINVAL;

	}
	return(0);
}

static int pinchar_mmap(struct file *file, struct vm_area_struct *vma)
{
	return 0;
}

static int pinchar_open(struct inode *inode, struct file *file)
{
    	printk("<PINCHAR> : Device opened....\n");
	index = 0;
    	return(0);
}

static int pinchar_close(struct inode *inode, struct file *file)
{
    printk("<PINCHAR> : Device is Closed....\n");
    //if(pinchar_buffer)
    //	kfree(pinchar_buffer);
    //pinchar_buffer=NULL;
    index = 0;
    return(0);
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
    printk("<PINCHAR> : Device Initialized......\n");
    //major_no = register_chrdev(0, DEVICE_NAME, &pinchar_fops);
    ret = misc_register(&pin_device);
    if(ret < 0)
    {
        printk("<PINCHAR> : Registration Failed....\n");
        return(-1);
    }
    printk("<PINCHAR> : Device Registered Successfully...!\n");
    //printk("<PINCHAR> : Major Number - %d\n",major_no);

    return(0);
}

static void __exit pinchar_exit(void)
{
    printk("<PINCHAR> : Device exited..\n");
    //unregister_chrdev(major_no, DEVICE_NAME);
    misc_deregister(&pin_device);
}


module_init(pinchar_init);
module_exit(pinchar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PINTU KUMAR");
MODULE_DESCRIPTION("pinchar Driver...!");


