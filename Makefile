KERNEL_BUILD := 
KERNEL_CROSS_COMPILE := 

obj-m += ric_mod.o

all:
	$(MAKE) ARCH=arm -C $(KERNEL_BUILD) CROSS_COMPILE=$(KERNEL_CROSS_COMPILE) M=$(PWD) modules

debug:
	KCPPFLAGS="-DDEBUG" $(MAKE) ARCH=arm -C $(KERNEL_BUILD) CROSS_COMPILE=$(KERNEL_CROSS_COMPILE) M=$(PWD) modules

clean:
	$(MAKE) ARCH=arm -C $(KERNEL_BUILD) M=$(PWD) clean 2> /dev/null
	rm -f modules.order *~
