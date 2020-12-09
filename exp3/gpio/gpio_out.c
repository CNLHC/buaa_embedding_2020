#include "GPIO_CTL.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main1();
int main2();

int main() {
  main1(); // 两灯交替
  main2(); // 全灯交替
  return 0;
}

//两灯交替闪烁
int main1() {
  int ledfd1 = 37;
  int ledfd2 = 32;
  int sed = 0;
  int value1;
  int value2;
  // Todo:打开设备
  gpio_open(ledfd1, SYSFS_GPIO_RST_DIR_VAL_OUT);
  gpio_open(ledfd2, SYSFS_GPIO_RST_DIR_VAL_OUT);

  for (sed = 0; sed < 4; sed++) {
    printf("round %d\n", sed);
    gpio_set_value(ledfd1, SYSFS_GPIO_RST_VAL_H);
    gpio_set_value(ledfd2, SYSFS_GPIO_RST_VAL_L);
    usleep(1000000); // 10^6 us = 1s
    value1 = gpio_get_value(ledfd1);
    value2 = gpio_get_value(ledfd2);
    printf("before: %d led value is %d\n", ledfd1, value1);
    printf("before: %d led value is %d\n", ledfd2, value2);
    // Todo:设置电平
    gpio_set_value(ledfd1, SYSFS_GPIO_RST_VAL_L);
    gpio_set_value(ledfd2, SYSFS_GPIO_RST_VAL_H);

    value1 = gpio_get_value(ledfd1);
    value2 = gpio_get_value(ledfd2);
    printf("after: %d led value is %d\n", ledfd1, value1);
    printf("after: %d led value is %d\n", ledfd2, value2);
    usleep(1000000);
  }
  // Todo:关闭设备
  gpio_close(ledfd1);
  gpio_close(ledfd2);
  return 0;
}

//全灯交替闪烁
int main2() {
  int led_num = 1;
  int i, j;
  int fd;
  int gpio_list[] = {35, 24, 30, 26, 27, 37, 32, 29};
  // 打开 7个GPIO 文件
  // Todo:打开 7个GPIO 文件代码
  for (i = 0; i < 7; i++) {
    gpio_open(gpio_list[i], SYSFS_GPIO_RST_DIR_VAL_OUT);
  }
  // Todo:实现交替闪烁，间隔为0.5s
  for (i = 0; i < 7; i++) {
    printf("now led %d\n", i);
    for (j = 0; j < 2; j++) {
      gpio_set_value(gpio_list[i], SYSFS_GPIO_RST_VAL_H);
      usleep(500000);
      gpio_set_value(gpio_list[i], SYSFS_GPIO_RST_VAL_L);
      usleep(500000);
    }
  }
  // Todo:关闭7个gpio
  for (i = 0; i < 7; i++) {
    gpio_close(gpio_list[i]);
  }
  return 0;
}
