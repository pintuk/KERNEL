#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kobject.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/workqueue.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/err.h>
#include <linux/xarray.h>

static DEFINE_XARRAY(xarr);

static int __init xarray_test_init(void)
{
	int i;
	void *entry;
	unsigned long index;

	pr_info("[xarray_test]: %s: module init\n", __func__);

	//Store the value in the Xarray
	for (i = 0; i <= 20; i++)
		xa_store(&xarr, i, xa_mk_value(i*i), GFP_KERNEL);

	//Retrieve the value from Xarray
	xa_for_each(&xarr, index, entry) {
		pr_info("[xarray_test]: Stored value at (%ld) = %ld\n", index, xa_to_value(entry));
		if ((index % 5) == 0) {
			xa_set_mark(&xarr, index, XA_MARK_1);
			pr_info("[xarray_test]: setting mark for index: %ld\n", index);
		}
	}

	pr_info("\n[xarray_test]: Display only marked entries:\n");
	xa_for_each_marked(&xarr, index, entry, XA_MARK_1) {
		pr_info("[xarray_test]: Marked value at (%ld) = %ld\n", index, xa_to_value(entry));
	}

	return 0;
}

static void __exit xarray_test_exit(void)
{
	pr_info("[xarray_test]: %s: module exit\n", __func__);

	xa_destroy(&xarr);
}

module_init(xarray_test_init);
module_exit(xarray_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pintu Kumar");
MODULE_DESCRIPTION("xarray test module");
