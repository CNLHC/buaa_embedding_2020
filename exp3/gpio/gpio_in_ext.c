#include "GPIO_CTL.h"
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

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

typedef struct {
  int key_num;
  int led_num;
} * key_led_pair, key_led_pair_struct;

void *button_monitor(void *arg) {
  int ret;
  fd_set fdset;
  int value;
  int keyfd;
  int ledfd;
  int key_num;
  int led_num;
  key_led_pair pair;

  pair = (key_led_pair)arg;
  key_num = pair->key_num;
  led_num = pair->led_num;

  ledfd = gpio_open(led_num, SYSFS_GPIO_RST_DIR_VAL_OUT);
  if (ledfd == -1) {
    printf("OPEN GPIO ERROR.\n");
  }
  keyfd = gpio_open(key_num, SYSFS_GPIO_RST_DIR_VAL_IN);
  if (keyfd == -1) {
    printf("OPEN GPIO ERROR.\n");
  }
  FD_ZERO(&fdset);
  //设置下降沿触发中断
  gpiokey_set_edge(key_num);
  char path[100];
  sprintf(path, "/sys/class/gpio/gpio%d/value", key_num);
  keyfd = open(path, O_RDONLY);
  if (keyfd == -1) {
    printf("open gpio value ERROR.\n");
    pthread_exit(NULL);
    return (NULL);
  }

  // gpio led init
  if (gpio_init(), 0)
    printf("init failed!\n");

  int led_val;
  while (1) {
    //使用select模型
    FD_SET(keyfd, &fdset);
    // Todo: select
    ret = select(keyfd + 1, &fdset, NULL, NULL, NULL);
    if (ret == 1) {
      // printf("Triggering interrupt. \n" );
      if (FD_ISSET(keyfd, &fdset)) {
        // printf("do what you like\n");
      }
      value = gpio_read_keyval(key_num);
      if (value == 0) {
        led_val = gpio_get_value(led_num);
        if (led_val == 1) {
          // Todo:
          gpio_set_value(led_num, SYSFS_GPIO_RST_VAL_L);
        } else {
          // Todo:
          gpio_set_value(led_num, SYSFS_GPIO_RST_VAL_H);
        }
      } else {
        continue;
      }
    } else {
      printf("select NULL.\n");
    }
    FD_ZERO(&fdset);
    usleep(500 * 1000);
  }
  close(keyfd);
  pthread_exit(NULL);
  return (NULL);
}

int main(int argc, char *argv[]) {
  int i;
  int ret;
  int gpio_out_list[] = {35, 24, 30, 26, 27, 37, 32, 29};
  int gpio_in_list[] = {36, 38, 39, 170, 175, 4, 169};
  pthread_t tid[7];
  key_led_pair_struct pair;

  for (i = 0; i < 7; i++) {
    pair.key_num = gpio_in_list[i];
    pair.led_num = gpio_out_list[i];
    ret = pthread_create(&tid[i], NULL, button_monitor, (void *)&pair);
    if (ret != 0) {
      printf("Creat thread failed\n");
      exit(-1);
    }
  }
  for (i = 0; i < 5; i++) {
    pthread_join(tid[i], NULL);
  }
  pthread_exit(NULL);
  return (0);
}
