all: prodcons.c
	@echo "Compiling prodcons. SCP this over from QEMU."
	gcc -m32 -o prodcons -std=gnu89 -Wall -I linux-2.6.23.1/include/ prodcons.c
