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

#define REG_DP_RAM0		0x10  //��һ������ܵļĴ���
#define REG_DP_RAM1		0x11  //�ڶ�������ܵļĴ���
#define REG_DP_RAM2		0x12  //����������ܵļĴ���
#define REG_DP_RAM3		0x13  //���ĸ�����ܵļĴ���
#define REG_DP_RAM4		0x14
#define REG_DP_RAM5		0x15
#define REG_DP_RAM6		0x16
#define REG_DP_RAM7		0x17

#define ZLG7290_LED_MAJOR	800  //���豸��
#define ZLG7290_LED_MINOR	0    //���豸��
#define ZLG7290_LED_DEVICES	1    //�豸����

#define WRITE_DPRAM _IO('Z', 0)

// ������豸�ṹ��
struct zlg7290
{
	struct i2c_client *client;    //struct i2c_client����һ�����ص�i2c�����ϵ�i2c���豸
	struct input_dev *input;      //struct input_dev����һ�������豸
	struct delayed_work work;     //δʹ�ã������ӳٹ����ĵ���
	unsigned long delay;          //δʹ�ã������ӳٹ����ĵ���
	
	struct cdev cdev;             //cdev�ַ��豸
};

// ������豸�ṹ��ȫ��ʵ��
struct zlg7290 *ptr_zlg7290;

// �����λͼ��ÿһ��bit����һ���ƹܣ���1:������0:��
//Todo������0x80����������ܵĵ�һ��
unsigned char bit_map[8] = {
	0x80,//��ȫ��
};

// �������ʾ0-F 16�����ַ���λͼ����
//Todo�� ����0xfc ���� ��0����0x0c ���� ��1������������1���ɣ�
unsigned char hex_map[16] = {
	0xfc, 0x0c, // ��ȫ
};

// ��i2c����д����
// ���㣺������������ȡֵ�ĺ���
/* ������
 * zlg7290 ������ܽṹ��ָ��
 * len ����д����ֽ�������byteΪ��λ��
 * retlen �� ����д����ֽ�����read������ʹ�ã���ע���Ǹ�ָ�룩
 * buf�� ��д������ָ�루�Ĵ�����ַ�ʹ�д���ݣ�
 */
static int zlg7290_hw_write(struct zlg7290 *zlg7290,  int len, size_t *retlen, char *buf)
{
	//
	struct i2c_client *client = zlg7290->client;
	int ret;

    //i2c_msg
    /* ������ 
     * client->addr : ���豸���ʵ�ַ
     * 0 ��flagλ��0����write��I2C_M_RD����read
     * len �� ��д����ֽ���
     * buf �� ��д������ָ�루�Ĵ�����ַ�ʹ�д���ݣ�
     */
	struct i2c_msg msg[] = {
		{ client->addr, 0, len, buf},
	};
    
	//Todo������i2c_transfer������������д���ݣ�ע�����
	//ret =i2c_transfer(client->adapter, Todo, Todo);
	if (ret < 0) 
	{
		dev_err(&client->dev, "i2c write error!\n");
		return -EIO;
	}

	*retlen = len;
	return 0;
}

// ��i2c���߶�����
static int zlg7290_hw_read(struct zlg7290 *zlg7290, int len, size_t *retlen, char *buf)
{
    //Todo������zlg7290_hw_writeд��read����
    /*���裺
     *1. ����i2c_msg msg[]������read��Ҫ����ͨ�ţ������Ҫ�������β�����ֵ��
         ��һ��i2c_msg����������Ҫ��ȡ�Ĵ��豸Ŀ��Ĵ����ĵ�ַ��buf��������Ĵ����ĵ�ַ
         �ڶ���i2c_msg�������ý��մ��豸Ŀ��Ĵ����������ݵ�buf�Լ����ݳ��ȣ�buf����ӼĴ����ڶ�����ֵ�ĵ�ַ
     *2. ����i2c_transfer(ע����������д)
     *3. ������
     */

}

// ��������豸
static int zlg7290_led_open(struct inode *inode, struct file *file)
{
	return 0;
}

// �ͷ�������豸
static int zlg7290_led_release(struct inode *inode, struct file *file)
{
	return 0;
}

// ���ϵĽӿڣ��û�̬ͨ������ioctlϵͳ��������ʽ���øú���
/* ������
 * filp ���ļ��ṹ�壬�û�̬Ϊ�ļ�������
 * cmd��
 * arg�� ��д���ݣ�long��ϵͳ��ռ8bytes����Ӧ8���Ĵ�����arg��λ��Ӧ�͵�ַ�Ĵ�����
 */
static long zlg7290_led_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	return 0;
}

// file_operations: ��ϵͳ���ú�����������������Ĺؼ����ݽṹ
static struct file_operations zlg7290_led_fops = {
	.owner = THIS_MODULE,
	.open = zlg7290_led_open,
	.release = zlg7290_led_release,
	.unlocked_ioctl = zlg7290_led_ioctl,
};

// ע�������С���豸���豸�ţ�MAJOR�����豸�ţ�MINOR
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

// ��ϵͳ��ע���豸
static int unregister_zlg7290_led(struct zlg7290 *zlg7290) 
{
	cdev_del(&zlg7290->cdev);
	
	unregister_chrdev_region(MKDEV(ZLG7290_LED_MAJOR, ZLG7290_LED_MINOR), ZLG7290_LED_DEVICES);
	
	return 0;
}

// ̽�⺯�����������ƥ�䵽��Ŀ���豸�����߻��Զ��ص�probe����
// ���㣺probe������remove����������󶨵ģ�
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
	
	//�����豸�ṹ������Ҫ�Ŀռ�
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
	
	
	// ע�������豸
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

// �ͷź��������ƥ�䵽���豸�������Ƴ��ˣ����߻��Զ��ص�remove����
static int zlg7290_remove(struct i2c_client *client) 
{
	struct zlg7290 *zlg7290 = i2c_get_clientdata(client);
    unregister_zlg7290_led(zlg7290);
    printk("remove done! \n");
	return 0;
}

// ƥ���豸������
// ���㣺Ϊʲô��Ҫi2c_device_id��
// ��ʾ��һ���豸ֻ��һ��������һ���������Զ�Ӧ����豸
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

// ��������ṹ�壨��Ҫ��
// ���㣺�����init��probe��remove��exit������ִ��˳��
// ��ʾ����ͨ��printk������
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

// ��ʼ��ģ��
static int __init zz_init(void)
{
	//module_i2c_driver��չ��
	i2c_register_driver(THIS_MODULE, &zlg7290_driver);

	//�ں�̬i2c_transferͨ��
	//Todo������8�������������ͬ��ֻʹ�õ�4������ѭ����ʾbit_map��hex_map�е������ַ�
	//��ʾ�� ����zlg7290_hw_write����
	int i;
    int j;
	ssize_t len = 0;

	unsigned char write_val[2] = {0};
    /* write_val[0] : ��д��Ĵ����ĵ�ַ
     * write_val[1] : ��д��Ĵ���������
     * ���磺write_val[0] = REG_DP_RAM0;  //д0�żĴ���
     *      write_val[1] = 0xfc;         //д���ַ���0��
     */
	for(i=0; i<8; i++){ //���ѭ����bit_map�ĳ���
		for(j = 0; j < 8; j++){// �ڲ�ѭ�� �� 8���Ĵ�����Ӧ8������ܣ���������ֻʹ�õ��˵�4λ�Ĵ���
			//Todo��

			msleep(1); //˯��1����
		}
		msleep(1000); //˯��1��
	}

    for(i=0; i<16; i++){ //���ѭ����hex_map�ĳ���
		for(j = 0; j < 8; j++){// �ڲ�ѭ�� �� 8���Ĵ�����Ӧ8������ܣ���������ֻʹ�õ��˵�4λ�Ĵ���
			//Todo��
	
			msleep(1); //˯��1����
		}
		msleep(1000); //˯��1��
	}
    
    for(j = 0; j < 8; j++){
			//Todo�� ���Ҫ���������������
	
			msleep(1);
		}
	return 0;
}

// ж��ģ��
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
