${CROSS_COMPILE}gcc -o gpio_out gpio_out.c
${CROSS_COMPILE}gcc -o gpio_in gpio_in.c
${CROSS_COMPILE}gcc -o gpio_in_ext gpio_in_ext.c -lpthread
