obj-m := ptree.o
KDIR := ~/linux-systems-lab02-addrices/kernel

all:
		make -C $(KDIR) M=$(PWD) modules

clean:
		make -C $(KDIR) M=$(PWD) clean
