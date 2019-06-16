
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>			/* for kmalloc() and kfree() */
#include <linux/uaccess.h>		/* for copy to/from users */
#include <linux/mman.h>			/* for mmap() operation */
#include <linux/io.h>
#include <linux/string.h>
#include <linux/ioctl.h>
#include <linux/cma.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/gfp.h>
#include <linux/nodemask.h>
#include <linux/delay.h>

#include <linux/scatterlist.h>
#include <linux/dma-buf.h>
#include <linux/dma-mapping.h>


#define DEVICE_NAME	"exporterA"
#define EXPORTERA_MAGIC   '+'

#define EXPORTERA_ALLOC		_IOWR(EXPORTERA_MAGIC, 1, int)
#define EXPORTERA_BUF_EXPORT	_IOWR(EXPORTERA_MAGIC, 2, int)
#define EXPORTERA_SHARE_FD	_IOWR(EXPORTERA_MAGIC, 3, int)
#define EXPORTERA_FREE		_IOWR(EXPORTERA_MAGIC, 4, int)



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


static int exporterA_attach(struct dma_buf *dmabuf, struct device *dev,
				struct dma_buf_attachment *attach)
{
	printk(KERN_INFO "<EXPORTERA>: exporterA_attach: called !\n");
	return 0;
}

static void exporterA_detach(struct dma_buf *dmabuf,
				struct dma_buf_attachment *attach)
{
	printk(KERN_INFO "<EXPORTERA>: exporterA_detach: called !\n");
	dma_buf_put(dmabuf);
}

static struct sg_table *
	exporterA_map_dmabuf(struct dma_buf_attachment *attach,
				enum dma_data_direction direction)
{
	struct sg_table *sgt;
	int ret;
	int val = 0;
	struct exporter_dmabuf_buf *expbuf;

	printk(KERN_INFO "<EXPORTERA>: exporterA_map_dmabuf: called !\n");
	if(!attach->dmabuf->priv) {
		printk(KERN_INFO "<EXPORTERA>: exporterA_map_dmabuf: attach->dmabuf->priv is NULL\n");
		return NULL;
	}
	expbuf = attach->dmabuf->priv;
	sgt = kzalloc(sizeof(struct sg_table), GFP_KERNEL);
	if (!sgt) {
		printk(KERN_ERR "<EXPORTERA>: failed to allocate sg table.\n");
		return ERR_PTR(-ENOMEM);
	}

	ret = sg_alloc_table(sgt, 1, GFP_KERNEL);
	if (ret < 0) {
		printk(KERN_ERR "<EXPORTERA>: failed to allocate scatter list.\n");
		kfree(sgt);
		sgt = NULL;
		return ERR_PTR(-ENOMEM);
	}

	sg_init_table(sgt->sgl, 1);
	sg_dma_len(sgt->sgl) = expbuf->size;
	sg_set_page(sgt->sgl, pfn_to_page(PFN_DOWN(expbuf->dma_addr)),
			expbuf->size, 0);
	sg_dma_address(sgt->sgl) = expbuf->dma_addr;

	/*
	 * increase reference count of this buf object.
	 *
	 * Note:
	 * alloated physical memory region is being shared with others so
	 * this region shouldn't be released until all references of this
	 * region will be dropped by vb2_cma_phys_unmap_dmabuf().
	 */
	val = atomic_inc_return(&expbuf->refcount);
	printk(KERN_INFO "<EXPORTERA>: exporterA_map_dmabuf: refcount: %d\n",
						val);

	return sgt;
}

static void exporterA_unmap_dmabuf(struct dma_buf_attachment *attach,
					struct sg_table *sgt)
{
	int val = 0;
	struct exporter_dmabuf_buf *expbuf;
	printk(KERN_INFO "<EXPORTERA>: exporterA_unmap_dmabuf: called !\n");

	expbuf = attach->dmabuf->priv;
	sg_free_table(sgt);
	kfree(sgt);
	sgt = NULL;

	if(!attach->dmabuf->priv) {
		printk(KERN_INFO "<EXPORTERA>: exporterA_unmap_dmabuf: attach->dmabuf->priv is NULL\n");
		return;
	}
	val = atomic_dec_return(&expbuf->refcount);
	printk(KERN_INFO "<EXPORTERA>: exporterA_unmap_dmabuf: refcount: %d\n",
						val);
}

static void exporterA_release(struct dma_buf *dmabuf)
{
	int val = 0;
	struct exporter_dmabuf_buf *expbuf;
	
	printk(KERN_INFO "<EXPORTERA>: exporterA_release: called !\n");
	if(!dmabuf->priv) {
		printk(KERN_INFO "<EXPORTERA>: exporterA_release: dmabuf->priv is NULL\n");
		return;
	}
	expbuf = dmabuf->priv;

	if (expbuf->export_dma_buf == dmabuf) {
		expbuf->export_fd = -1;
		expbuf->export_dma_buf = NULL;

		//if (atomic_dec_and_test(&expbuf->refcount)) 
		{
			//printk(KERN_INFO "<EXPORTERA>: exporterA_release: atomic_dec_and_test\n");
		if (expbuf->dma_addr != NULL) {
			cma_free(expbuf->dma_addr);
			printk(KERN_INFO "<EXPORTERA>: exporterA_release: cma free - Done!\n");
		}
		expbuf->dma_addr = NULL;
		}
		/*
		val = atomic_dec_return(&buf.refcount);
		printk(KERN_INFO "<EXPORTERA>: exporterA_release: refcount: %d\n",
						val);
		*/
	}
}

static void *exporterA_kmap_atomic_dmabuf(struct dma_buf *dma_buf,
						unsigned long page_num)
{
	return NULL;
}

static void exporterA_kunmap_atomic_dmabuf(struct dma_buf *dma_buf,
						unsigned long page_num,
						void *addr)
{

}

static void *exporterA_kmap_dmabuf(struct dma_buf *dma_buf,
					unsigned long page_num)
{
	return NULL;
}

static void exporterA_kunmap_dmabuf(struct dma_buf *dma_buf,
					unsigned long page_num, void *addr)
{

}

static struct dma_buf_ops exporterA_dmabuf_ops = {
	.attach		= exporterA_attach,
	.detach		= exporterA_detach,
	.map_dma_buf	= exporterA_map_dmabuf,
	.unmap_dma_buf	= exporterA_unmap_dmabuf,
	.release	= exporterA_release,
	.kmap		= exporterA_kmap_dmabuf,
	.kmap_atomic	= exporterA_kmap_atomic_dmabuf,
	.kunmap		= exporterA_kunmap_dmabuf,
	.kunmap_atomic	= exporterA_kunmap_atomic_dmabuf,
};



static ssize_t exporterA_read(struct file *file, char *buff,
				size_t length, loff_t *pos)
{
	printk(KERN_INFO "<EXPORTERA>: Reading....\n");
	return 0;
}

static ssize_t exporterA_write(struct file *file, const char *buff,
				size_t length, loff_t *pos)
{
	printk(KERN_INFO "<EXPORTERA>: Writing: len = %d\n", length);
	return 0;
}

static int exporterA_ioctl(struct file *file, unsigned int cmd,
				unsigned long arg)
{
	switch (cmd) {
	case EXPORTERA_ALLOC:
	{
		int i = 0;
		size_t len = 0;
		struct cma_info meminfo;
		int err = 0;
		int val = 0;
		unsigned char *exportdata;
		//struct exporter_dmabuf_buf *buf;
		//char msg[] = "This is exporter data";
		printk(KERN_INFO "<EXPORTERA>: exporterA_ioctl: Inside - EXPORTERA_ALLOC\n");
		get_user(len, (unsigned long __user *)arg);
		printk(KERN_INFO "EXPORTER BUFFER: len: %d\n", len);
		err = cma_info(&meminfo, buf->dev, 0);
		printk(KERN_INFO "<EXPORTERA>: CMA INFO: addr: %x ~ %x, total_size: 0x%x, free_size: 0x%x, count: %u\n",
			meminfo.lower_bound, meminfo.upper_bound,
			meminfo.total_size, meminfo.free_size, meminfo.count);
		buf->dma_addr = cma_alloc(buf->dev, NULL, len, 0);
		if (IS_ERR((void *)buf->dma_addr)) {
			printk(KERN_ERR "<EXPORTERA>: exporterA_ioctl: Inside - EXPORTERA_ALLOC: cma_alloc - Failed !\n");
			return -ENOMEM;
		}
		buf->size = len;
		exportdata = (unsigned char *)phys_to_virt(buf->dma_addr);
		memset(exportdata, 0xab, buf->size);
		//memcpy(exportdata, msg, strlen(msg));
		//exportdata[strlen(msg)] = '\0';
		printk(KERN_INFO "<EXPORTERA>: DATA: \n");
		
		for (i=0; i<10; i++) {
			printk(KERN_INFO "%x", exportdata[i]);
		}
		printk(KERN_INFO ".......\n");
		
		/*
		val = atomic_inc_return(&buf.refcount);
		printk(KERN_INFO "<EXPORTERA>: EXPORTERA_ALLOC: refcount: %d\n",
						val);
		*/
		break;
	}
	case EXPORTERA_BUF_EXPORT:
	{
		int val = 0;
		printk(KERN_INFO "<EXPORTERA>: exporterA_ioctl: Inside - EXPORTERA_BUF_EXPORT\n");
		buf->export_dma_buf = dma_buf_export(buf,
					&exporterA_dmabuf_ops,
					buf->size, 0600);
		if (!buf->export_dma_buf) {
			printk(KERN_ERR "<EXPORTERA>: exporterA_ioctl: EXPORTERA_BUF_EXPORT: dma_buf_export - Failed !\n");
			return -1;
		}
		buf->export_fd = dma_buf_fd(buf->export_dma_buf, 0);
		if (buf->export_fd < 0) {
			printk(KERN_ERR "<EXPORTERA>: exporterA_ioctl: EXPORTERA_BUF_EXPORT: dma_buf_fd - Failed !\n");
			dma_buf_put(buf->export_dma_buf);
			return -1;
		}
		printk(KERN_INFO "<EXPORTERA>: exported fd is: %d\n",
						buf->export_fd);
		/*
		val = atomic_inc_return(&buf.refcount);
		printk(KERN_INFO "<EXPORTERA>: EXPORTERA_BUF_EXPORT: refcount: %d\n",
							val);
		*/
		break;
	}
	case EXPORTERA_SHARE_FD:
	{
		int fd = buf->export_fd;
		printk(KERN_INFO "<EXPORTERA>: exporterA_ioctl: Inside - EXPORTERA_SHARE_FD\n");
		put_user(fd, (unsigned long __user *)arg);
		break;
	}
	case EXPORTERA_FREE:
	{
		//int val = 0;
		//printk(KERN_INFO "<EXPORTERA>: exporterA_ioctl: Inside - EXPORTERA_FREE\n");
		//if (buf->dma_addr)
		//	cma_free(buf->dma_addr);
		//buf->dma_addr = 0;
		/**
		val = atomic_dec_return(&buf.refcount);
		printk(KERN_INFO "<EXPORTERA>: EXPORTERA_FREE: refcount: %d\n",
							val);
		*/
		break;
	}
	default:
		printk(KERN_INFO "<EXPORTERA>: ioctl - default\n");
		return -EINVAL;

	} /*End of switch */
	return 0;
}

static int exporterA_mmap(struct file *file, struct vm_area_struct *vma)
{
	return 0;
}

static int exporterA_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "<EXPORTERA>: Device opened....\n");
	return 0;
}

static int exporterA_close(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "<EXPORTERA>: Device is Closed....\n");
	return 0;
}


static const struct file_operations exporterA_fops = {
	.owner = THIS_MODULE,
	.read  = exporterA_read,
	.write = exporterA_write,
	.unlocked_ioctl = exporterA_ioctl,
	.open  = exporterA_open,
	.mmap  = exporterA_mmap,
	.release = exporterA_close,
};

static struct miscdevice pin_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &exporterA_fops,
};



static int __init exporterA_init(void)
{
	int ret = 0;
	printk(KERN_INFO "<EXPORTERA>: Device Initialized......\n");
	ret = misc_register(&pin_device);
	if (ret < 0) {
		printk(KERN_INFO "<EXPORTERA>: Registration Failed....\n");
		return -1;
	}
	buf = kzalloc(sizeof(struct exporter_dmabuf_buf),GFP_KERNEL);
	if (!buf) {
		printk(KERN_INFO "<EXPORTERA>: kzalloc Failed !\n");
		return -1;
	}
	buf->dev = pin_device.this_device;
	printk(KERN_INFO "<EXPORTERA>: Device Registered Successfully...!\n");

	return 0;
}

static void __exit exporterA_exit(void)
{
	printk(KERN_INFO "<EXPORTERA>: Device exited..\n");
	misc_deregister(&pin_device);
	if(buf != NULL)
		kfree(buf);
	buf = NULL;
}


module_init(exporterA_init);
module_exit(exporterA_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PINTU KUMAR");
MODULE_DESCRIPTION("exporterA Driver...!");

