/*
 * BUAA LDMC
 */

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

#define ZLG7290_NAME		"zlg7290"
#define ZLG7290_LED_NAME	"zlg7290_led"

#define REG_DP_RAM0		0x10  //第一个数码管的寄存器
#define REG_DP_RAM1		0x11  //第二个数码管的寄存器
#define REG_DP_RAM2		0x12  //第三个数码管的寄存器
#define REG_DP_RAM3		0x13  //第四个数码管的寄存器
#define REG_DP_RAM4		0x14
#define REG_DP_RAM5		0x15
#define REG_DP_RAM6		0x16
#define REG_DP_RAM7		0x17

#define ZLG7290_LED_MAJOR	800  //主设备号
#define ZLG7290_LED_MINOR	0    //从设备号
#define ZLG7290_LED_DEVICES	1    //设备数量

#define WRITE_DPRAM _IO('Z', 0)

// 数码管设备结构体
struct zlg7290
{
	struct i2c_client *client;    //struct i2c_client代表一个挂载到i2c总线上的i2c从设备
	struct input_dev *input;      //struct input_dev代表一个输入设备
	struct delayed_work work;     //未使用，用于延迟工作的调度
	unsigned long delay;          //未使用，用于延迟工作的调度
	
	struct cdev cdev;             //cdev字符设备
};

// 数码管设备结构体全局实例
struct zlg7290 *ptr_zlg7290;


// 数码管显示A-Z 16进制字符的位图数据
//Todo： 例如0xfc 代表 “0”，0x0c 代表 “1”（左右两种1均可）
//    1a
//    -
// 2b| |6f
//    -7g
// 3c| |5e
//    -  .8dp
//   4d
int letter_number = 7; // A-G
unsigned char letter_map[7] = {
		0xee, 0x3e, 0x9c, 0x7a, 0x9e, 0x8e, 0xf8 // online website code
	// 0xee, 0x7a, 0xf0, 0x3e, 0xf2, 0xe2, 0xf8 // 补全, 这里不太确定,按照上方第一条为第一段, 之后逆时针为第二,三段, 最后中间的一条为第七段,  第八段 is dp
};

// 向i2c总线写数据
// 考点：分析各个参数取值的含义
/* 参数：
 * zlg7290 ：数码管结构体指针
 * len ：待写入的字节数（以byte为单位）
 * retlen ： 返回写入的字节数（read函数中使用）（注意是个指针）
 * buf： 待写入数据指针（寄存器地址和待写数据）
 */
static int zlg7290_hw_write(struct zlg7290 *zlg7290,  int len, size_t *retlen, char *buf)
{
	//
	struct i2c_client *client = zlg7290->client;
	int ret;

    //i2c_msg
    /* 参数： 
     * client->addr : 从设备访问地址
     * 0 ：flag位，0代表write，I2C_M_RD代表read
     * len ： 待写入的字节数
     * buf ： 待写入数据指针（寄存器地址和待写数据）
     */
	struct i2c_msg msg[] = {
		{ client->addr, 0, len, buf},
	};
    
	//Todo：调用i2c_transfer函数向总线上写数据，注意参数
	ret =i2c_transfer(client->adapter, msg, 1);
    //struct i2c_msg *msgs, int num;

	if (ret < 0) 
	{
		dev_err(&client->dev, "i2c write error!\n");
		return -EIO;
	}

	*retlen = len;
	return 0;
}

// 从i2c总线读数据
static int zlg7290_hw_read(struct zlg7290 *zlg7290, int len, size_t *retlen, char *buf)
{
    //Todo：仿照zlg7290_hw_write写出read函数
    /*步骤：
     *1. 构造i2c_msg msg[]（由于read需要两次通信，因此需要构造两次参数的值）
         第一个i2c_msg用来发送需要读取的从设备目标寄存器的地址，buf保存待读寄存器的地址
         第二个i2c_msg用来设置接收从设备目标寄存器返回数据的buf以及数据长度，buf保存从寄存器内读到的值的地址
     *2. 调用i2c_transfer(注意参数如何填写)
     *3. 错误处理
     */

	struct i2c_client *client = zlg7290->client;
	int ret;


    //i2c_msg
    /* 参数： 
     * client->addr : 从设备访问地址
     * 0 ：flag位，0代表write，I2C_M_RD代表read
     * len ： 待写入的字节数
     * buf ： 待写入数据指针（寄存器地址和待写数据）
     */
	struct i2c_msg msg[] = {
		{ client->addr, 0, 1, buf}, // write buf into device
		{ client->addr, I2C_M_RD, len, buf}, // read from device into data, then set buf as data pointer
	};
    
	ret =i2c_transfer(client->adapter, msg, 2);
	if (ret < 0) 
	{
		dev_err(&client->dev, "i2c read error!\n");
		return -EIO;
	}

	*retlen = len;
	return 0;

}

// 打开数码管设备
static int zlg7290_led_open(struct inode *inode, struct file *file)
{
	return 0;
}

// 释放数码管设备
static int zlg7290_led_release(struct inode *inode, struct file *file)
{
	return 0;
}

// 向上的接口，用户态通过调用ioctl系统函数，隐式调用该函数
/* 参数：
 * filp ：文件结构体，用户态为文件描述符
 * cmd：
 * arg： 待写数据（long在系统中占8bytes，对应8个寄存器，arg高位对应低地址寄存器）
 */
static long zlg7290_led_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	return 0;
}

// file_operations: 把系统调用和驱动程序关联起来的关键数据结构
static struct file_operations zlg7290_led_fops = {
	.owner = THIS_MODULE,
	.open = zlg7290_led_open,
	.release = zlg7290_led_release,
	.unlocked_ioctl = zlg7290_led_ioctl,
};

// 注册数码管小灯设备主设备号：MAJOR，从设备号：MINOR
static int register_zlg7290_led(struct zlg7290 *zlg7290) 
{
	struct cdev *zlg7290_cdev;
	int ret;
	dev_t devid;

	devid = MKDEV(ZLG7290_LED_MAJOR, ZLG7290_LED_MINOR);
	ret = register_chrdev_region(devid, ZLG7290_LED_DEVICES, ZLG7290_LED_NAME);
	if (ret < 0) {
		dev_err(&zlg7290->client->dev, "register chrdev fail!\n");
		return ret;
	}

	zlg7290_cdev = &zlg7290->cdev;
	cdev_init(zlg7290_cdev, &zlg7290_led_fops);
	zlg7290_cdev->owner = THIS_MODULE;
	ret = cdev_add(zlg7290_cdev, devid, 1);
	if (ret < 0) {
		dev_err(&zlg7290->client->dev, "cdev add fail!\n");
		goto err_unreg_chrdev;
	}
	return 0;

err_unreg_chrdev:
	unregister_chrdev_region(devid, ZLG7290_LED_DEVICES);
	return ret;
}

// 从系统中注销设备
static int unregister_zlg7290_led(struct zlg7290 *zlg7290) 
{
	cdev_del(&zlg7290->cdev);
	
	unregister_chrdev_region(MKDEV(ZLG7290_LED_MAJOR, ZLG7290_LED_MINOR), ZLG7290_LED_DEVICES);
	
	return 0;
}

// 探测函数，如果驱动匹配到了目标设备，总线会自动回调probe函数
// 考点：probe函数和remove函数在哪里绑定的？
static int 
zlg7290_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct input_dev *input_dev;
	int ret = 0;
	
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE)) {
		dev_err(&client->dev, "%s adapter not supported\n",
			dev_driver_string(&client->adapter->dev));
		return -ENODEV;
	}
	
	//申请设备结构体所需要的空间
	ptr_zlg7290 = kzalloc(sizeof(struct zlg7290), GFP_KERNEL);
	input_dev = input_allocate_device();
	if (!ptr_zlg7290 || !input_dev) {
		ret = -ENOMEM;
		goto err_free_mem;
	}
	
	input_dev->name = client->name;
	input_dev->phys = "zlg7290-keys/input0";
	input_dev->dev.parent = &client->dev;
	input_dev->id.bustype = BUS_I2C;
	input_dev->id.vendor = 0x0001;
	input_dev->id.product = 0x0001;
	input_dev->id.version = 0x0001;
	input_dev->evbit[0] = BIT_MASK(EV_KEY);
	
	
	// 注册输入设备
	ret = input_register_device(input_dev);
	if (ret) {
		dev_err(&client->dev, "unable to register input device\n");
		goto err_unreg_dev;
	}

	ptr_zlg7290->client = client;
	ptr_zlg7290->input = input_dev;
	i2c_set_clientdata(client, ptr_zlg7290);
	
	ret = register_zlg7290_led(ptr_zlg7290);
	if (ret < 0)
		return ret;

    printk("probe done! \n");
	return 0;

err_unreg_dev:
	input_unregister_device(input_dev);
	input_dev = NULL;
err_free_mem:
	input_free_device(input_dev);
	kfree(ptr_zlg7290);

	return ret;
}

// 释放函数，如果匹配到的设备从总线移除了，总线会自动回调remove函数
static int zlg7290_remove(struct i2c_client *client) 
{
	struct zlg7290 *zlg7290 = i2c_get_clientdata(client);
    unregister_zlg7290_led(zlg7290);
    printk("remove done! \n");
	return 0;
}

// 匹配设备和驱动
// 考点：为什么需要i2c_device_id？
// 提示：一个设备只有一个驱动，一个驱动可以对应多个设备
static const struct i2c_device_id zlg7290_id[] = {
	{ZLG7290_NAME, 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, zlg7290_id);

#ifdef CONFIG_OF
static const struct of_device_id zlg7290_dt_ids[] = {
	{ .compatible = "myzr,zlg7290", },
	{ }
};
MODULE_DEVICE_TABLE(of, zlg7290_dt_ids);
#endif

// 驱动程序结构体（重要）
// 考点：请分析init，probe，remove和exit函数的执行顺序
// 提示：可通过printk来分析
static struct i2c_driver zlg7290_driver= {
	.driver	= {
		.name	= ZLG7290_NAME,
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(zlg7290_dt_ids),
	},
	.probe		= zlg7290_probe,
    .remove     = zlg7290_remove,
	.id_table	= zlg7290_id
};

// 初始化模块
static int __init zz_init(void)
{
	//module_i2c_driver宏展开
	i2c_register_driver(THIS_MODULE, &zlg7290_driver);

	//内核态i2c_transfer通信
	//Todo：保持8个数码管内容相同（只使用到4个），循环显示letter_map中的所有字符
	//提示： 调用zlg7290_hw_write函数
	int i;
    int j;
	ssize_t len = 0;

	unsigned char write_val[2] = {0};
    /* write_val[0] : 待写入寄存器的地址
     * write_val[1] : 待写入寄存器的数据
     * 例如：write_val[0] = REG_DP_RAM0;  //写0号寄存器
     *      write_val[1] = 0xfc;         //写入字符“0”
     */

    unsigned char dirty_work[256] = {0};
    for(i=0; i<letter_number; i++){ //外层循环：letter_map的长度
        dirty_work[letter_map[i]] = i+'A';
    }

    for(i=0; i<letter_number; i++){ //外层循环：letter_map的长度
        write_val[1] = letter_map[i];
        printk("letter_id[%d]: ",i+1);
		for(j = 0; j < 8; j++){// 内层循环 ： 8个寄存器对应8个数码管，开发板上只使用到了低4位寄存器
			//Todo：
            size_t retlen;
            write_val[0] = REG_DP_RAM0 + i;
            zlg7290_hw_write(ptr_zlg7290,  2, &retlen, write_val);
            zlg7290_hw_read(ptr_zlg7290,  1, &retlen, write_val);
            printk("%c ", dirty_work[write_val[0]]);
			msleep(1); //睡眠1毫秒
		}
        printk("\n");
		msleep(1000); //睡眠1秒
	}
    
    write_val[1] = 0;
    for(j = 0; j < 8; j++){
			//Todo： 最后要将所有数码管置零
            size_t retlen;
            write_val[0] = REG_DP_RAM0 + i;
            zlg7290_hw_write(ptr_zlg7290,  2, &retlen, write_val);
	
			msleep(1);
		}
	return 0;
}

// 卸载模块
static void __exit zz_exit(void)
{	
    i2c_del_driver(&zlg7290_driver);
	printk("886, The module has been removed\n");
}

module_init(zz_init);
module_exit(zz_exit);

MODULE_AUTHOR("buaa ldmc");
MODULE_DESCRIPTION("Keypad & Leds driver for ZLG7290");
MODULE_LICENSE("GPL");
