ifneq ($(KERNELRELEASE),)
obj-m := inter_botton_syn.o
else
PWD := $(shell pwd)
KDIR := /home/healer/myshare/linuxkernel/linux-2.6.22.6
all:
	make -C $(KDIR) M=$(PWD)
clean:	
	rm -rf *.o *.ko *.mod.c *.symvers *.c~ *~
endif
