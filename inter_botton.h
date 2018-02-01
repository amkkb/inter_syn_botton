#ifndef _INTER_BOTTON_
#define _INTER_BOTTON_

#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/irq.h>
#include <linux/poll.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <asm/arch/regs-gpio.h>

#define KEY_DRIVER_MAJOR 0

struct global_dev
{
	struct cdev key_driver_cdev;/*cdev 结构体*/
	wait_queue_head_t wait_head; /*阻塞的等待队列头*/
	struct fasync_struct *inter_async_queue; /* 异步结构体指针 */
	
};
/*中断事件标志，中断中进行置1，read中将它清0*/
static volatile int en_press = 0;
/*声明用到的结构体和变量*/
static int key_driver_major = KEY_DRIVER_MAJOR;
dev_t key_dev_num;//dev_t类型设备号
static struct class *key_drv_class;
static struct class_device *key_drv_class_dev;
/*用于地址映射*/
volatile unsigned long *gpfcon;
volatile unsigned long *gpfdat;

volatile unsigned long *gpgcon;
volatile unsigned long *gpgdat;

#define rGPFCON 0X56000050
#define rGPGCON 0X56000060
/*用于描述按键值的结构体*/
struct pin_desc{
	unsigned int pin;
	unsigned char key_val;
};

#endif