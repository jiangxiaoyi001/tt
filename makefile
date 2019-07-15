KVERS=$(shell uname -r)
obj-m += globalmem.o
build:kernel_modules
kernel_modules:
	make -C /lib/modules/$(KVERS)/build/ M=$(CURDIR) modules
clean:
	make -C /lib/modules/$(KVERS)/build/ M=$(CURDIR) clean
