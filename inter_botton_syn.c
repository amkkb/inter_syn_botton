#include "inter_botton.h"

struct global_dev *global_devp;


static unsigned char key_val = 0;//按键值

struct pin_desc pin_desc_val[] = {
		{S3C2410_GPF0, 0X01},
		{S3C2410_GPF2, 0X02},
		{S3C2410_GPG3, 0X03},
		{S3C2410_GPG11,0X04},
};

static int key_driver_read (struct file * file, char __user * buf, size_t size, loff_t *ppos)
{
	
	int ret;
	printk("debug\n");
	if(size != 1)
		return -EINVAL;

	ret = copy_to_user(buf,&key_val,1);
	en_press = 0;	
	
	return 1;

}

static irqreturn_t button_irq(int irq, void *dev_id)
{	
	struct pin_desc *p = (struct pin_desc *)dev_id; 
	unsigned int pin_val;
	pin_val = s3c2410_gpio_getpin(p->pin);
	key_val = 0;
	if(pin_val)
		{
			/*松开*/
			key_val = 0x80 | p->key_val;
			//printk("kernel key_val  = 0x%x\n",key_val);//调试用的
		}	
	else
		{
			/*按下*/
			key_val = p->key_val;
			//printk("kernel anxia key_val  = 0x%x\n",key_val);//调试用的
		}

	en_press = 1; //表示中断发生
	kill_fasync(&global_devp->inter_async_queue, SIGIO, POLL_IN);
	return IRQ_HANDLED;
}

static unsigned int key_driver_poll(struct file *fp, poll_table * wait)
{
	unsigned int mask = 0;
	

	poll_wait(fp, &global_devp->wait_head, wait);
	if(en_press)
		{	
			mask |= POLLIN | POLLRDNORM;
		}
	return mask;
}


static int key_driver_open(struct inode *node, struct file *file)
{
	
	
	/*中断注册函数*/
	request_irq(IRQ_EINT0,button_irq,IRQT_BOTHEDGE,"S2",&pin_desc_val[0]);
	request_irq(IRQ_EINT2,button_irq,IRQT_BOTHEDGE,"S3",&pin_desc_val[1]);
	request_irq(IRQ_EINT11,button_irq,IRQT_BOTHEDGE,"S4",&pin_desc_val[2]);
	request_irq(IRQ_EINT19,button_irq,IRQT_BOTHEDGE,"S5",&pin_desc_val[3]);
		
	return 0;
}

static int key_driver_fasync (int fd, struct file *filp, int on)

{
	return fasync_helper (fd, filp, on, &global_devp->inter_async_queue);
}

/*文件操作结构体*/
static const struct file_operations key_driver_fops =
{
  .owner   = THIS_MODULE,
  .read    = key_driver_read,
  .open    = key_driver_open,
  .poll	   = key_driver_poll,
  .fasync  = key_driver_fasync,
};


/*初始化并注册 cdev*/
static void global_setup_cdev(struct global_dev *dev, int index)
{
	int err;
	int devno = MKDEV(key_driver_major, index);

	cdev_init(&dev->key_driver_cdev, &key_driver_fops);
	dev->key_driver_cdev.owner = THIS_MODULE;
	dev->key_driver_cdev.ops = &key_driver_fops;
	err = cdev_add(&dev->key_driver_cdev, devno, 1);
	if (err)
		printk(KERN_NOTICE "Error %d adding LED%d", err, index);
}


/*设备驱动模块加载函数*/
int key_driver_init(void)
{
  int result;
  int err;
  key_dev_num = MKDEV(key_driver_major, 0);

  /* 申请设备号*/
  if (key_driver_major)
    result = register_chrdev_region(key_dev_num, 1, "key_driver");
  else  /* 动态申请设备号 */
  {
    result = alloc_chrdev_region(&key_dev_num, 0, 1, "key_driver");
    key_driver_major = MAJOR(key_dev_num);
  }  
  if (result < 0)
    return result;
  
/* 动态申请设备结构体的内存*/
global_devp = kmalloc(sizeof(struct global_dev), GFP_KERNEL);
if (!global_devp) /*申请失败*/
{
	result = - ENOMEM;
	//goto fail_malloc;
}
memset(global_devp, 0, sizeof(struct global_dev));  
    
global_setup_cdev(global_devp, 0);    
  
init_waitqueue_head(&global_devp->wait_head); /*初始化读等待队列头*/
  
/*
* 为设备创建类
* 加载驱动模块时在/sys/class   目录下自动创建car_class  文件夹
*/
key_drv_class = class_create(THIS_MODULE, "key_driver");
if(IS_ERR(key_drv_class))
{
	printk("cannot create key_drv_class!\n");
	return 0;
}
/*创建设备文件节点，避免需要手动创建*/
key_drv_class_dev = class_device_create(key_drv_class, NULL,key_dev_num, NULL, "key_driver");

/*将GPIO  的物理地址映射到内核空间*/
gpfcon = (volatile unsigned long *)ioremap(rGPFCON, 16);
gpfdat = gpfcon + 1;

gpgcon = (volatile unsigned long *)ioremap(rGPGCON, 16);
gpgdat = gpgcon + 1;


return 0;

}
/*模块卸载函数*/
void key_driver_exit(void)
{

  cdev_del(&global_devp->key_driver_cdev);   /*注销cdev*/
  unregister_chrdev_region(key_dev_num, 1); /*释放设备号*/

  /*删除设备文件和dev/class  目录下的相应文件夹*/
	class_device_unregister(key_drv_class_dev);
	class_destroy(key_drv_class);
	/*解除GPIO  映射*/
	iounmap(gpfcon);
	iounmap(gpgcon);
  
}



MODULE_AUTHOR("healer");
MODULE_LICENSE("Dual BSD/GPL");

module_init(key_driver_init);
module_exit(key_driver_exit);