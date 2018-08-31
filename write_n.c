#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/sched.h>

MODULE_LICENSE("GPLv2");
MODULE_LICENSE("Sahan Fernando sahan.h.fernando@gmail.com");
MODULE_DESCRIPTION("Makes file of max length n");

// file descriptors mean nothing, seek heads always only point to 0
static int max_length=6;
static char *buf;
static struct mutex buf_mutex;

module_param(max_length, int, S_IRUSR | S_IRGRP);
MODULE_PARM_DESC(max_length, "Maximum Length of the buffer");

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

static ssize_t write_n_read(struct file *file, char __user *out, size_t size, loff_t *off) {
    loff_t new_pos;
    size_t to_write, written;
    mutex_lock(&buf_mutex);
    new_pos = MIN(max_length, *off + size);
    to_write = new_pos - *off;
    written = 0;
    while(*off < new_pos) {
        to_write = copy_to_user(out+written, buf+*off, to_write);
        // check if write to userspace succeeded
        if(to_write < 0) {
            mutex_unlock(&buf_mutex);
            return -EFAULT;
        }
        written += new_pos - to_write - *off;
        *off = new_pos - to_write;
    }
    mutex_unlock(&buf_mutex);
    return written;
}

static int write_n_open(struct inode *inode, struct  file *file) {
    // Do nothing?
    return 0;
}

static ssize_t write_n_write(struct file * file, const char __user * out, size_t size, loff_t * off) {
    loff_t new_pos;
    size_t to_write, written;
    mutex_lock(&buf_mutex);
    new_pos = MIN(max_length, *off + size);
    to_write = new_pos - *off;
    written = 0;
    while(*off < new_pos) {
        to_write = copy_from_user(buf+*off, out+written, to_write);
        // check if write to userspace succeeded
        if(to_write < 0) {
            mutex_unlock(&buf_mutex);
            return -EFAULT;
        }
        written += new_pos - to_write - *off;
        *off = new_pos - to_write;
    }
    mutex_unlock(&buf_mutex);
    return written;
}

static struct file_operations write_n_ops = {
    .owner = THIS_MODULE,
    .open = write_n_open,
    .llseek = noop_llseek,
    .write = write_n_write,
    .read = write_n_read
};

static struct miscdevice write_n_dev = {
    .minor=MISC_DYNAMIC_MINOR,
    .name="write_n",
    .fops=&write_n_ops
};

static int __init write_n_init(void)
{
    size_t i;
    printk(KERN_INFO "Creating write_n\n");
    buf = kmalloc(max_length, GFP_KERNEL);
    if(buf == NULL) {
        printk(KERN_INFO "write_n: failed to allocate buffer\n");
        return 1;
    }
    for(i = 0; i < max_length; ++i) {
        buf[i] = 0;
    }
    mutex_init(&buf_mutex);
    misc_register(&write_n_dev);
    return 0;
}

static void __exit write_n_cleanup(void)
{
    printk(KERN_INFO "Removing write_n\n");
    misc_deregister(&write_n_dev);
    mutex_destroy(&buf_mutex);
    kfree(buf);
}

module_init(write_n_init);
module_exit(write_n_cleanup);
