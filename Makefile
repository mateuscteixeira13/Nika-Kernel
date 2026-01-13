CFLAGS = -ffreestanding -m32 -O2 -Wall -Wextra -Werror -fno-stack-protector -Wno-unused-variable -fno-pie -no-pie -Iinc
OBJ_DIR = obj/
CC = gcc
NASM = nasm
AS = as

C_SOURCES := $(wildcard core/*.c) \
             $(wildcard core/io/*.c) \
             $(wildcard drivers/*.c) \
             $(wildcard lib/*.c) \
             $(wildcard memory/*.c) \
             $(wildcard process/*.c) \
             $(wildcard fs/*.c) \
             $(wildcard userland/*.c)

C_OBJS := $(addprefix $(OBJ_DIR),$(notdir $(C_SOURCES:.c=.o)))

ASM_NASM_SOURCES := $(wildcard asm/*.asm)
ASM_NASM_OBJS := $(addprefix $(OBJ_DIR),$(notdir $(ASM_NASM_SOURCES:.asm=.o)))

ASM_GAS_SOURCES := $(wildcard asm/*.s)
ASM_GAS_OBJS := $(addprefix $(OBJ_DIR),$(notdir $(ASM_GAS_SOURCES:.s=.o)))

ALL_OBJS := $(C_OBJS) $(ASM_NASM_OBJS) $(ASM_GAS_OBJS)

all: obj_dirs core.sys iso

obj_dirs:
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)%.o: core/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)%.o: core/io/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)%.o: drivers/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)%.o: lib/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)%.o: memory/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)%.o: process/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)%.o: fs/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)%.o: userland/%.c
	$(CC) $(CFLAGS) -c $< -o $@


$(OBJ_DIR)%.o: asm/%.asm
	$(NASM) -f elf32 $< -o $@

$(OBJ_DIR)%.o: asm/%.s
	$(AS) --32 $< -o $@

core.sys: $(ALL_OBJS)
	ld -m elf_i386 -T linker.ld -o $@ $(ALL_OBJS)

iso: core.sys
	mkdir -p iso/boot/grub iso/boot/kernel
	cp grub.cfg iso/boot/grub/
	cp core.sys iso/boot/kernel/core.sys
	grub-mkrescue -o nika.iso iso/
	qemu-system-i386 -cdrom nika.iso -display sdl -serial stdio

clean:
	rm -rf $(OBJ_DIR)*.o core.sys iso nika.iso

.PHONY: all obj_dirs iso clean