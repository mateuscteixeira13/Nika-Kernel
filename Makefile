CFLAGS = -ffreestanding -m32 -c -O3 -Iinc -Wall -Werror -Wextra
SRC_DIR = src/
OBJ_DIR = obj/
CC = gcc

all:
	mkdir -p obj/

	$(CC) $(CFLAGS) $(SRC_DIR)kernel.c -o $(OBJ_DIR)kernel.o
	$(CC) $(CFLAGS) $(SRC_DIR)hal.c -o $(OBJ_DIR)hal.o
	$(CC) $(CFLAGS) $(SRC_DIR)stdio.c -o $(OBJ_DIR)stdio.o
	$(CC) $(CFLAGS) $(SRC_DIR)string.c -o $(OBJ_DIR)string.o
	$(CC) $(CFLAGS) $(SRC_DIR)pmm.c -o $(OBJ_DIR)pmm.o
	$(CC) $(CFLAGS) $(SRC_DIR)pic.c -o $(OBJ_DIR)pic.o
	$(CC) $(CFLAGS) $(SRC_DIR)irq.c -o $(OBJ_DIR)irq.o
	$(CC) $(CFLAGS) $(SRC_DIR)asm.c -o $(OBJ_DIR)asm.o
	$(CC) $(CFLAGS) $(SRC_DIR)idt.c -o $(OBJ_DIR)idt.o
	$(CC) $(CFLAGS) $(SRC_DIR)irq.c -o $(OBJ_DIR)irq.o
	$(CC) $(CFLAGS) $(SRC_DIR)fork.c -o $(OBJ_DIR)fork.o
	$(CC) $(CFLAGS) $(SRC_DIR)heap.c -o $(OBJ_DIR)heap.o
	$(CC) $(CFLAGS) $(SRC_DIR)paging.c -o $(OBJ_DIR)paging.o
	$(CC) $(CFLAGS) $(SRC_DIR)pf.c -o $(OBJ_DIR)pf.o
	$(CC) $(CFLAGS) $(SRC_DIR)ramfs.c -o $(OBJ_DIR)ramfs.o
	
	as -32 $(SRC_DIR)entry.s -o $(OBJ_DIR)entry.o
	nasm -f elf32 $(SRC_DIR)idt_stub.asm -o $(OBJ_DIR)idt_stub.o
	nasm -f elf32 $(SRC_DIR)irq_stub.asm -o $(OBJ_DIR)irq_stub.o


	ld -m elf_i386 -T linker.ld -o core.sys $(OBJ_DIR)*.o

	mkdir -p iso/boot/grub/
	mkdir -p iso/boot/kernel/

	cp -r grub.cfg iso/boot/grub/
	cp -r core.sys iso/boot/kernel/core.sys

	grub-mkrescue -o nika.iso iso/


	qemu-system-i386 -cdrom nika.iso

clean:
	rm -rf obj/* *.sys
