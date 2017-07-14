Linux_Dir=/home/ken/nanopi2/linux-3.4.y/

CC=arm-linux-gnueabihf-gcc

PWD := $(shell pwd)

AIM=spidriver.o

all:
	make -C $(Linux_Dir) M=$(PWD) modules
	@echo "------------- module sucess ----------------"

clean:
	make -C $(Linux_Dir) M=$(PWD) modules clean
	rm -rf modules.order

obj-m += $(AIM)
