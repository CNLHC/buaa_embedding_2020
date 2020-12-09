#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>

#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/gpio/consumer.h>

#define GPIO_lednum 37 //gpio_led 编号
#define GPIO_keynum 36 //gpio_key 编号
MODULE_AUTHOR("mq");
MODULE_LICENSE("GPL");


static int cout = 0;//记录中断触发次数

static irqreturn_t irq_interrupt(int irq,void *dev_id)
{
	irqreturn_t irqreturn = IRQ_HANDLED;
	int ret;
	int led_num = 37;
	//每执行一次中断，就对cout进行++，根据中断次数对gpio_led进行控制
	/*由于无法使用gpio_get_value来获取准备的值，根据触发中断的次数来对gpio_led进行控制，每触发一次中断，对cout++，偶数次设置高电平，奇数次设置低电平*/

	cout++;
	/*******************申请一个gpio led***********************************/
	//Todo：
	ret = gpio_request(led_num, "GPIO_led");
	if(ret < 0)
	{
		printk(KERN_ALERT"GPIO%d_request error!",led_num);
		return -1;
	}
	printk(KERN_ALERT"the irq num is %d.\n",irq);
	//设置gpio引脚的值，对目前引脚值进行取反
	/*？？？
	gpio引脚的直总是0，set_value(1)之后，读取到的仍然是0
	*/
	ret = gpio_get_value(led_num);
	printk(KERN_ALERT"befor GPIO%d_value is %d.\n",led_num,ret);//print the value
	if(cout%2 == 0)
	{
		//Todo：设置gpio方向为out，初始值为1
		gpio_direction_output(led_num, 1);
		printk(KERN_ALERT"after GPIO%d_value is %d.\n",led_num,gpio_get_value(led_num));
	}else if (cout%2 == 1)
	{
		//Todo：设置gpio方向为out，初始值为0
		gpio_direction_output(led_num, 0);
		printk(KERN_ALERT"after GPIO%d_value is %d.\n",led_num,gpio_get_value(led_num));
	}
	printk(KERN_ALERT"The interrupt occures! \n");
	gpio_free(led_num);

	return irqreturn;
}
static int led_up(int led_num, int key_num)
{
	int ret;
	int irq;
	int i;

	//Todo：申请一个gpio key
	ret = gpio_request(key_num, "GPIO_key");
	if(ret < 0)//申请失败
	{
		printk(KERN_ALERT"GPIO%d_request error!\n",key_num);
		return -1;
	}
	//Todo：设置gpio方向为in
	ret = gpio_direction_input(key_num);
	if(ret < 0)
	{
		printk(KERN_ALERT"GPIO%d_set_direction error!",key_num);
		return -1;
	}else
		printk(KERN_ALERT"gpio%d set direction!\n",key_num);
	//Todo：设置中断
	irq = gpio_to_irq(key_num);
	printk(KERN_ALERT"begin the irq num is %d.\n",irq);
	if(irq != -1)
	{

		//注册中断函数irq_process()，中断号为key_num，设置下降沿触发
		ret = request_irq(irq, irq_interrupt,IRQF_TRIGGER_FALLING, "GPIO_test",NULL);
		if(ret == -1)
		{
			printk(KERN_ALERT"requeset_irq error!\n");
		}
		else
			printk(KERN_ALERT"request_irq is done.\n");

		printk(KERN_ALERT"sleep 2000ms.\n");
		msleep(2000);

		for(i=0;i<10;i++)
		{
			//设置延迟，使程序存活20s
			msleep(2000);
			printk(KERN_ALERT"sleep 2000ms.\n");

		}
		//Todo：释放中断信号
		free_irq(irq, NULL);

		//Todo：释放gpio
		gpio_free(key_num);

	}else
	{
		printk(KERN_ALERT"irq_requeset failed\n");
	}
	return 1;
}
/******     初始化模块    *******/
static int __init mq_init(void)
{
	int ret;
	printk(KERN_ALERT "hello,the module has been installed\n");
	ret = led_up(GPIO_lednum,GPIO_keynum);
	if(ret<0)
		printk("the module_install failed!\n");
	return 0;
}
/******退出模块，卸载模块时，运行exit函数*******/
static void __exit mq_exit(void)
{
	printk("The module has been removed\n");
}


module_init(mq_init);
module_exit(mq_exit);
