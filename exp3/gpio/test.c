#include "GPIO_CTL.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
  int i;
  int fd;
  int gpio_out_list[] = {35, 24, 30, 26, 27, 37, 32, 29};
  int gpio_in_list[] = {36, 38, 39, 170, 175, 4, 169};

  for (i = 0; i < 8; i++) {
    fd = gpio_open(gpio_out_list[i], SYSFS_GPIO_RST_DIR_VAL_OUT);
    if (fd == -1) {
      printf("OPEN %d GPIO LED ERROR.\n", gpio_out_list[i]);
    }
  }
  for (i = 0; i < 7; i++) {
    fd = gpio_open(gpio_in_list[i], SYSFS_GPIO_RST_DIR_VAL_IN);
    if (fd == -1) {
      printf("OPEN %d GPIO BUTTON ERROR.\n", gpio_in_list[i]);
    }
  }
  return 0;
}
