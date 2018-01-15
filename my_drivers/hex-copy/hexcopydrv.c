
/*
 * hexcopy.c (convert binary file to hex file)
 *
 * Copyright (C) 2017 XYZ INC.
 * Author: Pintu Kumar <pintu_agarwal@yahoo.com>
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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>
#include <linux/mutex.h>
#include <linux/ctype.h>


#define DEVICE_NAME	"hexcopy"

static DEFINE_MUTEX(lock);
static loff_t offset;
static struct file *fp = NULL;

static void open_file(const char *path)
{
	mm_segment_t old_fs;
	//struct file *fp = NULL;

	old_fs = get_fs();
	set_fs(KERNEL_DS);

	fp = filp_open(path, O_CREAT | O_RDWR | O_TRUNC, 0666);
	if (IS_ERR(fp)) {
		pr_err("[HEXCOPY]:%s: Failed to open file: /tmp/output\n",
				__func__);
		return;
	}
	//if (!(IS_ERR(fp)))
	//	filp_close(fp, NULL);

	set_fs(old_fs);
}

static void close_file(struct file *fp)
{
	mm_segment_t old_fs;

	old_fs = get_fs();
	set_fs(KERNEL_DS);
	if (!(IS_ERR(fp)))
		filp_close(fp, NULL);
	set_fs(old_fs);
}

static void hex_dump_each_line(const void *buf, size_t len, int rowsize,
			char *linebuf, size_t linebuflen)
{
	const unsigned char *ptr = buf;
	u8 ch;
	int j, lx = 0;

	if (!len)
		goto none;
	if (len > rowsize)	/* limit to one line at a time */
		len = rowsize;

	for (j = 0; (j < len) && (lx + 3) <= linebuflen; j++) {
		ch = ptr[j];
		linebuf[lx++] = hex_asc_hi(ch);
		linebuf[lx++] = hex_asc_lo(ch);
		linebuf[lx++] = ' ';
	}
	if (j)
		lx--;
	
none:
	linebuf[lx++] = '\0';
}

static void do_hex_dump(int rowsize, const void *buf, size_t len)
{
	const unsigned char *ptr = buf;
	int i, linelen, remaining = len;
	unsigned char linebuf[rowsize * 3 + 2];
	//struct file *fp = NULL;
	mm_segment_t old_fs;
	int ret = 0;
	char tmp[32];

	old_fs = get_fs();
	set_fs(KERNEL_DS);
#if 0
	fp = filp_open("/tmp/output", O_CREAT | O_RDWR, 0666);
	if (IS_ERR(fp)) {
		pr_err("[HEXCOPY]:%s: Failed to open file: /tmp/output\n",
				__func__);
		goto out1;
	}
#endif
	if (IS_ERR(fp)) {
		pr_err("[HEXCOPY]: Failed to access: /tmp/output\n");
		goto out1;
	}
	for (i = 0; i < len; i += rowsize) {
		linelen = min(remaining, rowsize);
		remaining -= rowsize;

		hex_dump_each_line(ptr + i, linelen, rowsize,
				   linebuf, sizeof(linebuf));
		/* print buffer to kernel logs */
		//pr_info("%s\n", linebuf);
		sprintf(tmp, "\n");
		strcat(linebuf, tmp);
		ret = vfs_write(fp, (char __user *)linebuf, strlen(linebuf),
					&offset);
		if (ret < 0) {
			pr_err("[HEXCOPY]: %s: Failed vfs_write\n", __func__);
			goto out2;
		}
	}
out2:
	//if (!(IS_ERR(fp)))
	//	filp_close(fp, NULL);

out1:
	set_fs(old_fs);
}

static ssize_t hexcopy_read(struct file *file, char __user *buff,
				size_t length, loff_t *pos)
{
	pr_info("[HEXCOPY]: Reading....\n");
	return 0;
}

static ssize_t hexcopy_write(struct file *file, const char __user *buff,
				size_t length, loff_t *pos)
{
	int ret = 0;
	int rowsize = 8;
	void *buffer;

	mutex_lock(&lock);
	//ret = length;
	//pr_info("[HEXCOPY]: write: length: %u\n", length);
	buffer = vmalloc(length);
	if (buffer == NULL) {
		pr_err("[HEXCOPY]: vmalloc failed (size: %u)\n", length);
		ret = -ENOMEM;
		goto out1;
	}
	memset(buffer, 0x00, length);
	if (copy_from_user(buffer, buff, length) != 0) {
		pr_err("[HEXCOPY]: copy_from_user failed...!\n");
		ret = -EFAULT;
		goto out2;
	}

	/* This function dump hex buffer to a file with row length 8 */
	do_hex_dump(rowsize, buffer, length);

	/* The following function can be used dto dump the hex output
	 * to the kernel log buffer directly, with 16 length, instead
	 * of reinventing the wheel.
	*/
	/*
	print_hex_dump(KERN_INFO, "", DUMP_PREFIX_NONE,
				16, 1, buffer, len, false);
	*/
	ret = length;

out2:
	vfree(buffer);
out1:
	mutex_unlock(&lock);
	return ret;
}

static int hexcopy_open(struct inode *inode, struct file *file)
{
	pr_info("[HEXCOPY]: Device opened....\n");
	offset = 0;
	/* trancate file if already exists */
	open_file("/tmp/output");

	return 0;
}

static int hexcopy_close(struct inode *inode, struct file *file)
{
	pr_info("[HEXCOPY]: Device is Closed....\n");

	close_file(fp);

	return 0;
}


static const struct file_operations hexcopy_fops = {
	.owner		= THIS_MODULE,
	.read		= hexcopy_read,
	.write		= hexcopy_write,
	.open		= hexcopy_open,
	.release	= hexcopy_close,
};

static struct miscdevice hexcopydevice = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= DEVICE_NAME,
	.fops	= &hexcopy_fops,
	.mode	= 0666,
};


static int __init hexcopy_init(void)
{
	int ret = -1;
	ret = misc_register(&hexcopydevice);
	if (ret < 0) {
		pr_err("[HEXCOPY]: Registration Failed....\n");
		return -1;
	}
	pr_info("[HEXCOPY]: Device Registered Successfully...!\n");
	return 0;
}

static void __exit hexcopy_exit(void)
{
	pr_info("[HEXCOPY] : Device exited..\n");
	misc_deregister(&hexcopydevice);
}


module_init(hexcopy_init);
module_exit(hexcopy_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("PINTU KUMAR");
MODULE_DESCRIPTION("hexcopy Driver...!");

