obj-m      := kfssuper.o fssuper.o
lskfs-objs := kfssuper.o fssuper.o

KERNEL_SOURCE ?= /lib/modules/$(shell uname -r)/build

PWD := $(shell pwd)

all: test

test: compile-module

compile-module:
	$(MAKE) -C $(KERNEL_SOURCE) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_SOURCE) M=$(PWD) clean
