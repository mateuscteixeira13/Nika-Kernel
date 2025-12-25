#include <stdint.h>
#include <vga.h>
#include <fork.h>
#include <mem.h>
#include <paging.h>

void page_fault_handler(uint32_t error, uint32_t addr){
    if(error & 1){
        panic("Protection fault");
    }

    uint32_t page = (uint32_t)pmm_alloc_page();
    if(!page){
        panic("Out of memory");
    }

    map_page(current_process->page_dir, addr & 0xFFFFF000, page, PAGE_RW | PAGE_USER);

    asm volatile("invlpg (%0)" :: "r"(addr) : "memory");
}
