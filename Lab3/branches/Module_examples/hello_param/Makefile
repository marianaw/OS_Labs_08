KERNELDIR ?= "/lib/modules/$(shell uname -r)/build"
PWD := "$(shell pwd)"

obj-m := hello_param.o
module-objs := hello_param.o

default:
	$(MAKE) -C "$(KERNELDIR)" "M=$(PWD)" modules

clean:
	rm -Rf $(OBJECTS) *.ko *.o *.mod.* *.symvers .*o.cmd .tmp_versions/ *~
