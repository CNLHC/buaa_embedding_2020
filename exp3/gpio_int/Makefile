obj-m	:= gpio_in_modu.o


#KERNELDIR ?= $(HOME)/my-imx6/02_sources/linux-3.14.52-myimx6a9
KERNELDIR := /home/lhm/my-imx6-1/02_sources/linux-3.14.52
PWD       := $(shell pwd)

default:
	make -C $(KERNELDIR) M=$(PWD) MOD_INC=$(PWD) modules ARCH=arm CROSS_COMPILE=/home/lhm/my-imx6-1/03_tools/gcc-linaro-5.4.1-2017.05-i686_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-


clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.symvers *.order
