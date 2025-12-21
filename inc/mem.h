#ifndef MEM_H
#define MEM_H

#include <stdint.h>
#include <multiboot.h>

#define PAGE_SIZE 4096

int pmm_init(multiboot_info_t* mbd, uint32_t magic);
void pmm_mark_free(uint32_t addr);
void* pmm_alloc_page();
void pmm_reserve_kernel();

#endif