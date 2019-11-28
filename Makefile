obj-m += rootkit.o 
rootkit-y := rtk.o fdlib.o hooklib.o

all: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
