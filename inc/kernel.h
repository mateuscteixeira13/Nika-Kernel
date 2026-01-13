#pragma once

#include <hal.h>
#include <mem.h>
#include <stdint.h>
#include <multiboot.h>
#include <pic.h>
#include <idt.h>
#include <irq.h>
#include <heap.h>
#include <paging.h>
#include <vga.h>
#include <ramfs.h>
#include <kbd.h>
#include <gdt.h>
#include <pit.h>
#include <gd.h>
#include <mouse.h>
#include <syscall.h> // Just here! I do nothing!
#include <pci.h>

extern volatile uint32_t *vga;