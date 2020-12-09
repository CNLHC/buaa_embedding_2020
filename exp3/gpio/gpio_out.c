#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>
#include <unistd.h>
#include <fcntl.h>   
#include "GPIO_CTL.h"

//两灯交替闪烁
int main()
{
	int ledfd1 = 37;
	int ledfd2 = 32;
	int sed = 0;
	int value1;
	int value2;
	//Todo:打开设备

	for(sed=0;sed<4;sed++)
	{
		printf("round %d\n",sed);
		gpio_set_value(ledfd1,SYSFS_GPIO_RST_VAL_H);
		gpio_set_value(ledfd2,SYSFS_GPIO_RST_VAL_L);
		usleep(1000000);
		value1 = gpio_get_value(ledfd1);
		value2 = gpio_get_value(ledfd2);
		printf("before: %d led value is %d\n",ledfd1,value1 );
		printf("before: %d led value is %d\n",ledfd2,value2 );
		//Todo:设置电平

		value1 = gpio_get_value(ledfd1);
		value2 = gpio_get_value(ledfd2);
		printf("after: %d led value is %d\n",ledfd1,value1 );
		printf("after: %d led value is %d\n",ledfd2,value2 );

		usleep(1000000);
	}
	//Todo:关闭设备

}
/*
//全灯交替闪烁
int main()
{
	int led_num = 1;
	int i,j;
	int fd;
	打开 7个GPIO 文件
   	//Todo:打开 7个GPIO 文件代码
	//Todo:实现交替闪烁，间隔为0.5s
	
	//Todo:关闭7个gpio

	return 0;
}
*/
