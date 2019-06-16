
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>		/* for kmalloc() and kfree() */
#include <linux/uaccess.h>	/* for copy to/from users */
#include <linux/mman.h>		/* for mmap() operation */
#include <linux/io.h>
#include <linux/string.h>
#include <linux/ioctl.h>

#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/gfp.h>
#include <linux/nodemask.h>
#include <linux/delay.h>

#include <linux/scatterlist.h>
#include <linux/dma-buf.h>
#include <linux/dma-mapping.h>


#define DEVICE_NAME	"importerB"
#define IMPORTERB_MAGIC   '-'

#define IMPORTERB_BUF_GET	_IOWR(IMPORTERB_MAGIC, 1, int)
#define IMPORTERB_BUF_MAP	_IOWR(IMPORTERB_MAGIC, 2, int)
#define IMPORTERB_BUF_UNMAP	_IOWR(IMPORTERB_MAGIC, 3, int)



struct exporter_dmabuf_buf {
	struct device *dev;
	dma_addr_t dma_addr;
	size_t size;
	int export_fd;
	struct dma_buf *export_dma_buf;
	struct dma_buf_attachment *db_attach;
	atomic_t refcount;
};
static struct exporter_dmabuf_buf *buf;


static ssize_t importerB_read(struct file *file, char *buff,
				size_t length, loff_t *pos)
{
	printk(KERN_INFO "<IMPORTERB>: Reading....\n");
	return 0;
}

static ssize_t importerB_write(struct file *file, const char *buff,
				size_t length, loff_t *pos)
{
	printk(KERN_INFO "<IMPORTERB>: Writing....len = %d\n", length);
	return 0;
}

static int importerB_ioctl(struct file *file, unsigned int cmd,
				unsigned long arg)
{
	switch (cmd) {
	case IMPORTERB_BUF_GET:
	{
		int fd = 0;
		printk(KERN_INFO "<IMPORTERB>: importerB_ioctl: Inside - IMPORTERB_BUF_GET\n");
		get_user(fd, (unsigned long __user *)arg);
		printk(KERN_INFO "<IMPORTERB>: importerB_ioctl: Inside - IMPORTERB_BUF_GET: fd: %d\n",
							fd);
		buf->export_dma_buf = dma_buf_get(fd);
		if (IS_ERR_OR_NULL(buf->export_dma_buf)) {
			printk(KERN_ERR "<IMPORTERB> : ERROR: dma_buf_get : Failed !\n");
			return -EINVAL;
		}
		printk(KERN_INFO "<IMPORTERB>: importerB_ioctl: Inside - IMPORTERB_BUF_GET: dma_buf_get - Done !\n");
		buf->db_attach = dma_buf_attach(buf->export_dma_buf, buf->dev);
		if (IS_ERR(buf->db_attach)) {
			printk(KERN_ERR "<IMPORTERB> : importerB_ioctl : IMPORTERB_BUF_GET : dma_buf_attach - Failed !\n");
			return -EINVAL;
		}
		printk(KERN_INFO "<IMPORTERB>: importerB_ioctl: IMPORTERB_BUF_GET - Done !\n");
		break;
	}
	case IMPORTERB_BUF_MAP:
	{
		struct sg_table *sg;
		unsigned char *data;
		int i = 0;
		size_t len = 0;
		
		printk(KERN_INFO "<IMPORTERB>: importerB_ioctl: Inside - IMPORTERB_BUF_MAP\n");
		get_user(len, (unsigned long __user *)arg);
		sg = dma_buf_map_attachment(buf->db_attach, DMA_BIDIRECTIONAL);
		if (!sg) {
			printk(KERN_ERR "<IMPORTERB>: importerB_ioctl: Inside - IMPORTERB_BUF_MAP: dma_buf_map_attachment - Failed !\n");
			return -EINVAL;
		}
		buf->dma_addr = sg_dma_address(sg->sgl);
		buf->size = sg_dma_len(sg->sgl);
		printk(KERN_INFO "<IMPORTERB>: IMPORTER WRITTEN: len: %d\n",
						buf->size);
		data = (unsigned char *)phys_to_virt(buf->dma_addr);
		//memset(data, 0xef,  buf->size);
		printk(KERN_INFO "<IMPORTERB>: DATA: \n");
		
		for (i=0; i<10; i++) {
			printk(KERN_INFO "%x", data[i]);
		}
		printk(KERN_INFO "........\n");
		
		buf->export_dma_buf->priv = sg;
		break;
	}
	case IMPORTERB_BUF_UNMAP:
	{
		struct sg_table *sg;
		printk(KERN_INFO "<IMPORTERB>: importerB_ioctl: Inside - IMPORTERB_BUF_UNMAP\n");
		sg = buf->export_dma_buf->priv;
		dma_buf_unmap_attachment(buf->db_attach, sg, DMA_BIDIRECTIONAL);
		buf->dma_addr = 0;
		buf->size = 0;
		dma_buf_detach(buf->db_attach->dmabuf, buf->db_attach);
		printk(KERN_INFO "<IMPORTERB>: IMPORTERB_BUF_UNMAP: dma_buf_detach: Done !\n");
		buf->db_attach = NULL;
		break;
	}
	default:
		printk(KERN_INFO "<IMPORTERB>: ioctl - default\n");
		return -EINVAL;

	} /* End of switch */
	return 0;
}

static int importerB_mmap(struct file *file, struct vm_area_struct *vma)
{
	return 0;
}

static int importerB_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "<IMPORTERB> : Device opened....\n");
	return 0;
}

static int importerB_close(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "<IMPORTERB> : Device is Closed....\n");
	return 0;
}


static const struct file_operations importerB_fops = {
	.owner = THIS_MODULE,
	.read  = importerB_read,
	.write = importerB_write,
	.unlocked_ioctl = importerB_ioctl,
	.open  = importerB_open,
	.mmap  = importerB_mmap,
	.release = importerB_close,
};

static struct miscdevice pin_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &importerB_fops,
};



static int __init importerB_init(void)
{
	int ret = 0;
	printk(KERN_INFO "<IMPORTERB>: Device Initialized......\n");
	ret = misc_register(&pin_device);
	if (ret < 0) {
		printk(KERN_INFO "<IMPORTERB>: Registration Failed....\n");
		return -1;
	}
	buf = kzalloc(sizeof(struct exporter_dmabuf_buf),GFP_KERNEL);
	if (!buf) {
		printk(KERN_INFO "<IMPORTERB>: kzalloc - Failed !\n");
		return -1;
	}
	buf->dev = pin_device.this_device;
	printk(KERN_INFO "<IMPORTERB>: Device Registered Successfully...!\n");
	return 0;
}

static void __exit importerB_exit(void)
{
	printk(KERN_INFO "<IMPORTERB>: Device exited..\n");
	misc_deregister(&pin_device);
	if (buf!= NULL)
		kfree(buf);
	buf = NULL;
}


module_init(importerB_init);
module_exit(importerB_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PINTU KUMAR");
MODULE_DESCRIPTION("importerB Driver...!");

