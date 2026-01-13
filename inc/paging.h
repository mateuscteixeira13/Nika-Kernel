#pragma once

#include <stdint.h>
#include <fork.h>

// Page declarations
#define PAGE_PRESENT 0x1
#define PAGE_RW 0x2
#define PAGE_USER 0x4
#define PAGE_WRITE_THROUGH 0x008 

#define PAGE_ENTRIES 1024

#define KERNEL_BASE 0xC0000000
#define KERNEL_PD_INDEX (KERNEL_BASE >> 22)

uint32_t *create_page_table();
void map_page(uint32_t* pd, uint32_t virt, uint32_t phys, uint32_t flags);
void init_pages();
void clone_kernel_space(uint32_t *new_pd);
void copy_page(uint32_t src_phys, uint32_t dst_phys);
void copy_addr_space(process_t *parent, process_t *child);
void map_physical_region(uint32_t* pd, uint32_t phys_start, uint32_t size, uint32_t flags);
void map_framebuffer(uint32_t fb_addr, uint32_t fb_size);

void map_user_space(uint32_t* pd, uint32_t code_addr, uint32_t code_size, 
                    uint32_t stack_addr, uint32_t stack_size);