KERNELDIR ?= "/lib/modules/$(shell uname -r)/build"
PWD := "$(shell pwd)"
OBJECTS := main.o fifo_ops.o  fifo.o rbuffer/rbuffer.o

obj-m := devfifo.o
devfifo-objs := $(OBJECTS)

default:
	$(MAKE) -C "$(KERNELDIR)" "M=$(PWD)" modules
	
clean:
	rm -Rf $(OBJECTS) *.ko *.o *.mod.* *.symvers .*o.cmd .tmp_versions/ *~
