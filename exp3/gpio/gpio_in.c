#include "GPIO_CTL.h"
#include "constant.h"
#include "util.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

void build_map() {
  for (int i = 0; i < KEY_COUNTS; i++)
    bind_key_led(GPIO_KEY_NUMBER[i], GPIO_LED_NUMBER[i]);
}

int gpio_read_keyval(int gpio_pin) {
  int fd;
  char GPIO_VAL[100];
  char value_str[3];

  sprintf(GPIO_VAL, "/sys/class/gpio/gpio%d/value", gpio_pin);
  fd = open(GPIO_VAL, O_RDONLY);
  if (fd == -1) {
    printf("OPEN FIEL GPIO VALUE ERROR.\n");
    return EXIT_FAILURE;
  }

  if (read(fd, value_str, 3) < 0) {
    printf("READ GPIO VALUE ERROR\n");
    return EXIT_FAILURE;
  }
  close(fd);

  // return the value of gpio
  if (value_str[0] == '1') {
    return 1;
  } else if (value_str[0] == '0') {
    return 0;
  } else
    return -1;
}
int gpiokey_set_edge(int gpio_pin) {
  int fd;
  char GPIO_EDGE[100];
  char value_str[10];
  sprintf(GPIO_EDGE, "/sys/class/gpio/gpio%d/edge", gpio_pin);
  printf("%s\n", GPIO_EDGE);
  fd = open(GPIO_EDGE, O_WRONLY);
  if (fd == -1) {
    printf("open gpio edge ERROR.\n");
    return EXIT_FAILURE;
  }
  if (write(fd, SYSFS_GPIO_EDGE_VAL, sizeof(SYSFS_GPIO_EDGE_VAL)) < 0)
    printf("write gpio edge ERROR.\n");
  read(fd, value_str, 10);
  printf("The edge is %s\n", value_str);
  close(fd);
}

int main(int argc, char *argv[]) {
  int ret;
  fd_set fdset;
  int value;
  int led_num = 24; /* gpio LED 的编号 */
  int key_num = 36; /* gpio KEY 的编号 */
  int keyfd[KEY_COUNTS];
  /*
   *打开led的gpio文件，设置为输出*/
  //补充代码
  int ledfd[KEY_COUNTS];
  for (int i = 0; i < KEY_COUNTS; i++) {
    ledfd[i] = gpio_open(GPIO_LED_NUMBER[i], SYSFS_GPIO_RST_DIR_VAL_OUT);
    if (ledfd == -1) {
      printf("OPEN GPIO ERROR.\n");
    }
    keyfd[i] = gpio_open(GPIO_KEY_NUMBER[i], SYSFS_GPIO_RST_DIR_VAL_IN);
    if (keyfd == -1) {
      printf("OPEN GPIO ERROR.\n");
    }
    gpiokey_set_edge(GPIO_KEY_NUMBER[i]);
    char path[100];
    sprintf(path, "/sys/class/gpio/gpio%d/value", GPIO_KEY_NUMBER[i]);
    keyfd[i] = open(path, O_RDONLY);
    if (keyfd == -1) {
      printf("open gpio value ERROR.\n");
      return EXIT_FAILURE;
    }
  }

  FD_ZERO(&fdset);
  //设置下降沿触发中断

  if (gpio_init(), 0)
    printf("init failed!\n");

  int led_val;

  while (1) {
    //使用select模型

    int maxfd = 0;
    for (int i = 0; i < KEY_COUNTS; i++) {
      FD_SET(keyfd[i], &fdset);
      if (maxfd < keyfd[i])
        maxfd = keyfd[i];
    }
    // Todo: select
    ret = select(maxfd + 1, &fdset, NULL, NULL, NULL);

    if (ret == -1) {
      printf("select err:%s\n", strerror(errno));
    }
    if (ret < 7) {
      for (int i = 0; i < KEY_COUNTS; i++) {
        if (FD_ISSET(keyfd[i], &fdset)) {
          printf("Triggering interrupt. %d\n", ret);
          int keynum = GPIO_KEY_NUMBER[i];
          value = gpio_read_keyval(keynum);
          if (value == 0) {
            led_val = gpio_get_value(key_to_led(keynum));
            if (led_val == 1) {
              gpio_set_value(key_to_led(keynum), SYSFS_GPIO_RST_VAL_L);
            } else {
              // Todo:
              gpio_set_value(key_to_led(keynum), SYSFS_GPIO_RST_VAL_H);
            }
          } else {
            continue;
          }
        }
      }

    } else {
      printf("select NULL.\n");
    }
    FD_ZERO(&fdset);
    usleep(500 * 1000);
  }
  close(keyfd);
  return (0);
}
