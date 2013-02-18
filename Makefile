all:
	mkdir -p .tmp_versions
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) obj-m=main.o main.ko

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
