obj-m	:= gpio_in_modu.o


#KERNELDIR ?= $(HOME)/my-work/02_sources/linux-3.14.52-myimx6a9
KERNELDIR ?= /home/yourname/my-imx6/02_sources/linux-3.14.52
PWD       := $(shell pwd)

default:
	make -C $(KERNELDIR) M=$(PWD) MOD_INC=$(PWD) modules


clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.symvers *.order
