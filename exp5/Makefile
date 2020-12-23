obj-m	:= myzr_zlg7290_write.o myzr_zlg7290_read_extention.o


#KERNELDIR ?= $(HOME)/my-work/02_sources/linux-3.14.52-myimx6a9
KERNELDIR ?= /root/my-imx6/02_sources/linux-3.14.52
PWD       := $(shell pwd)

default:
	make -C $(KERNELDIR) M=$(PWD) MOD_INC=$(PWD) modules


clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.symvers *.order
