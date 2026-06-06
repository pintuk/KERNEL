// SPDX-License-Identifier: GPL-2.0

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define HOG_DURATION_SEC 10
#define MEM_MB 256

static struct task_struct *cpu_thread;
static struct workqueue_struct *psi_hog_wq;

/* ------------------------------------------------ */
/* CPU HOG KTHREAD */
/* ------------------------------------------------ */

static int cpu_hog_fn(void *data)
{
    unsigned long end = jiffies + HOG_DURATION_SEC * HZ;

    pr_info("psi_cpu_hog started\n");

    while (!kthread_should_stop() && time_before(jiffies, end)) {
        cpu_relax();
    }

    pr_info("psi_cpu_hog finished\n");

    return 0;
}

/* ------------------------------------------------ */
/* MEMORY HOG WORK */
/* ------------------------------------------------ */

static void mem_hog_work(struct work_struct *work)
{
    char *buf;
    size_t sz = MEM_MB * 1024 * 1024;

    pr_info("psi_mem_hog_wq started\n");

    buf = vmalloc(sz);

    if (!buf)
        return;

    for (size_t i = 0; i < sz; i += PAGE_SIZE)
        buf[i] = 1;

    msleep(HOG_DURATION_SEC * 1000);

    vfree(buf);

    pr_info("psi_mem_hog_wq finished\n");
}

static DECLARE_WORK(mem_work, mem_hog_work);

/* ------------------------------------------------ */
/* IO HOG WORK */
/* ------------------------------------------------ */

static void io_hog_work(struct work_struct *work)
{
    struct file *f;
    loff_t pos = 0;
    char *buf;

    pr_info("psi_io_hog_wq started\n");

    buf = kmalloc(4096, GFP_KERNEL);

    if (!buf)
        return;

    memset(buf, 'X', 4096);

    f = filp_open("/tmp/kernel_io_hog.bin",
                  O_CREAT | O_WRONLY | O_SYNC,
                  0644);

    if (IS_ERR(f)) {
        kfree(buf);
        return;
    }

    for (int i = 0; i < 50000; i++)
        kernel_write(f, buf, 4096, &pos);

    filp_close(f, NULL);
    kfree(buf);

    pr_info("psi_io_hog_wq finished\n");
}

static DECLARE_WORK(io_work, io_hog_work);

/* ------------------------------------------------ */
/* MODULE INIT */
/* ------------------------------------------------ */

static int __init psi_hogger_init(void)
{
    pr_info("psi_kernel_hogger loaded\n");

    cpu_thread = kthread_run(cpu_hog_fn,
                             NULL,
                             "psi_cpu_hog");

    psi_hog_wq = alloc_workqueue("psi_hog_wq",
                                 WQ_UNBOUND,
                                 0);

    if (!psi_hog_wq)
        return -ENOMEM;

    queue_work(psi_hog_wq, &mem_work);
    queue_work(psi_hog_wq, &io_work);

    return 0;
}

/* ------------------------------------------------ */
/* MODULE EXIT */
/* ------------------------------------------------ */

static void __exit psi_hogger_exit(void)
{
    if (cpu_thread)
        kthread_stop(cpu_thread);

    flush_workqueue(psi_hog_wq);
    destroy_workqueue(psi_hog_wq);

    pr_info("psi_kernel_hogger unloaded\n");
}

module_init(psi_hogger_init);
module_exit(psi_hogger_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pintu Kumar Agarwal");
MODULE_DESCRIPTION("Kernel PSI workload hogger");


