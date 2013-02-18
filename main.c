/*  hello-1.c - The simplest kernel module.
 *
 *  Copyright (C) 2001 by Peter Jay Salzman
 *
 *  08/02/2006 - Updated by Rodrigo Rubira Branco <rodrigo@kernelhacking.com>
 */

/* Kernel Programming */
// #define MODULE
#define LINUX
//#define __KERNEL__


#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/serial.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/usb.h>
#include <linux/usb/serial.h>

#define VENDOR_WRITE_REQUEST_TYPE	0x40
#define VENDOR_WRITE_REQUEST		0x01

#define VENDOR_READ_REQUEST_TYPE	0xc0
#define VENDOR_READ_REQUEST		0x01

/* GPIO ioctl magic number */
#define PL2303_GPIO_MAGIC    'v'
/* GPIO ioctl NR field */
#define PL2303_GPIO_NR        0xac

static int pl2303_vendor_read(__u16 value, __u16 index,
		struct usb_serial *serial, unsigned char *buf)
{
	int res = usb_control_msg(serial->dev, usb_rcvctrlpipe(serial->dev, 0),
			VENDOR_READ_REQUEST, VENDOR_READ_REQUEST_TYPE,
			value, index, buf, 1, 100);
	return res;
}

static int pl2303_vendor_write(__u16 value, __u16 index,
		struct usb_serial *serial)
{
	int res = usb_control_msg(serial->dev, usb_sndctrlpipe(serial->dev, 0),
			VENDOR_WRITE_REQUEST, VENDOR_WRITE_REQUEST_TYPE,
			value, index, NULL, 0, 100);

	return res;
}

static int pl2303_gpio_ioctl(struct usb_serial_port* port,
                             unsigned int cmd, unsigned long arg)
{
    int value;
    unsigned char buf[10];
    struct usb_serial* serial = NULL;

    if(port)
        serial = port->serial;
    if(NULL == serial)
        return -EINVAL;

    /* check if it is a gpio ioctl */
    if((PL2303_GPIO_MAGIC != _IOC_TYPE(cmd)) ||
       (PL2303_GPIO_NR != _IOC_NR(cmd)))
        return -ENOIOCTLCMD;

    /* check argument size */
    if(sizeof(value) != _IOC_SIZE(cmd))
        return -EINVAL;

    memset(buf, 0, sizeof(buf));

    switch(_IOC_DIR(cmd)) {
    case _IOC_READ:
        /* read GPIO register */
        if(pl2303_vendor_read(0x0081, 0, serial, buf) < 1)
            return -EIO;
        value = buf[0];
        if(copy_to_user((void __user *)arg, &value, sizeof(value)))
            return -EFAULT;
        break;
    case _IOC_WRITE:
        if(copy_from_user(&value, (void __user*)arg, sizeof(value)))
            return -EFAULT;
        if(pl2303_vendor_write(1, value, serial))
            return -EIO;
        break;
    default:
        return -EINVAL;
    }
    return 0;
}

//extern struct usb_serial *usb_serial_get_by_index(unsigned int minor);

static int __init my_init_module(void)
{
        struct usb_serial* serial;
        printk("<1>Hello world 2.\n");
        
//        serial = usb_serial_get_by_index(0);
        
   // A non 0 return means init_module failed; module can't be loaded.
   return 0;
}

static void __exit my_cleanup_module(void)
{
  printk(KERN_ALERT "Goodbye world 2.\n");
}  

module_init(my_init_module);
module_exit(my_cleanup_module);




MODULE_LICENSE("GPL");
