KERNELDIR ?= "/lib/modules/$(shell uname -r)/build"
PWD := "$(shell pwd)"
OBJECTS := nuloops.o main.o

obj-m := nulo.o
nulo-objs := $(OBJECTS)

default:
	$(MAKE) -C "$(KERNELDIR)" "M=$(PWD)" modules
	
clean:
	rm -Rf $(OBJECTS) *.ko *.o *.mod.* *.symvers .*o.cmd .tmp_versions/ *~
