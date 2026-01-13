#include <stdint.h>
#include <mem.h>
#include <paging.h>
#include <fork.h>
#include <heap.h>
#include <serial.h>
#include <hal.h>
#include <vga.h>
/**
 * Bro, this sucked my energy LOL .-.
 */

uint32_t kernel_page_dir_phys;
uint32_t* kernel_page_dir;
uint32_t* kernel_page_tables[PAGE_ENTRIES] __attribute__((aligned(4096)));

uint32_t *create_page_table(){
    uint32_t *table = (uint32_t*) pmm_alloc_page();
    for(int i = 0; i < PAGE_ENTRIES; i++){
        table[i] = 0;
    }
    return table;
}

void map_page(uint32_t* pd, uint32_t virt, uint32_t phys, uint32_t flags){
    uint32_t pd_index = virt >> 22;
    uint32_t pt_index = (virt >> 12) & 0x3FF;
    
    if(!(pd[pd_index] & PAGE_PRESENT)){
        uint32_t* pt = create_page_table();
        pd[pd_index] = ((uint32_t)pt) | PAGE_PRESENT | PAGE_RW | PAGE_USER;
    }
    
    uint32_t pt_phys = pd[pd_index] & 0xFFFFF000;
    uint32_t* pt = (uint32_t*)pt_phys;
    pt[pt_index] = phys | (flags & 0xFFF) | PAGE_PRESENT;
}

void map_physical_region(uint32_t* pd, uint32_t phys_start, uint32_t size, uint32_t flags){
   
    uint32_t phys_aligned = phys_start & 0xFFFFF000;

    uint32_t pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    pages += ((phys_start - phys_aligned) + PAGE_SIZE - 1) / PAGE_SIZE;
    
    for(uint32_t i = 0; i < pages; i++){
        uint32_t addr = phys_aligned + (i * PAGE_SIZE);
        map_page(pd, addr, addr, flags);
    }
}

void map_framebuffer(uint32_t fb_addr, uint32_t fb_size){

    uint32_t flags = PAGE_RW | PAGE_WRITE_THROUGH;
    
    map_physical_region(kernel_page_dir, fb_addr, fb_size, flags);

    __asm__ volatile(
        "mov %%cr3, %%eax\n"
        "mov %%eax, %%cr3\n"
        ::: "eax"
    );
}

void map_user_space(uint32_t* pd, uint32_t code_addr, uint32_t code_size, 
                    uint32_t stack_addr, uint32_t stack_size) {
   
    uint32_t code_pages = (code_size + PAGE_SIZE - 1) / PAGE_SIZE;
    for(uint32_t i = 0; i < code_pages; i++){
        uint32_t virt = code_addr + (i * PAGE_SIZE);
        uint32_t phys = (uint32_t)pmm_alloc_page();
        map_page(pd, virt, phys, PAGE_RW | PAGE_USER);
    }
    

    uint32_t stack_pages = (stack_size + PAGE_SIZE - 1) / PAGE_SIZE;
    for(uint32_t i = 0; i < stack_pages; i++){
        uint32_t virt = stack_addr - ((i + 1) * PAGE_SIZE);  
        uint32_t phys = (uint32_t)pmm_alloc_page();
        map_page(pd, virt, phys, PAGE_RW | PAGE_USER);
    }
}

void clone_kernel_space(uint32_t *new_pd){
    for(uint32_t i = KERNEL_PD_INDEX; i < 1024; i++){
        new_pd[i] = kernel_page_dir[i];
    }
}

void copy_page(uint32_t src_phys, uint32_t dst_phys){
    uint8_t *src = (uint8_t*)src_phys;
    uint8_t *dst = (uint8_t*)dst_phys;
    for(int i = 0; i < PAGE_SIZE; i++){
        dst[i] = src[i];
    }
}

void copy_addr_space(process_t *parent, process_t *child){
    uint32_t *src_pd = parent->page_dir;
    uint32_t *dst_pd = child->page_dir;
    
    for(uint32_t pd_i = 0; pd_i < KERNEL_PD_INDEX; pd_i++){
        if(!(src_pd[pd_i] & PAGE_PRESENT))
            continue;
        
        uint32_t src_pt_phys = src_pd[pd_i] & 0xFFFFF000;
        uint32_t *src_pt = (uint32_t*)src_pt_phys;
        
        uint32_t *dst_pt = (uint32_t*)pmm_alloc_page();
        memset(dst_pt, 0, PAGE_SIZE);
        dst_pd[pd_i] = ((uint32_t)dst_pt) | PAGE_PRESENT | PAGE_RW | PAGE_USER;
        
        for(uint32_t pt_i = 0; pt_i < 1024; pt_i++){
            if(!(src_pt[pt_i] & PAGE_PRESENT))
                continue;
            
            uint32_t src_page = src_pt[pt_i] & 0xFFFFF000;
            uint32_t dst_page = (uint32_t)pmm_alloc_page();
            
            copy_page(src_page, dst_page);
            dst_pt[pt_i] = dst_page | PAGE_PRESENT | PAGE_RW | PAGE_USER;
        }
    }
}

void init_pages(){
   
    kernel_page_dir_phys = (uint32_t) pmm_alloc_page();
    kernel_page_dir = (uint32_t*) kernel_page_dir_phys;
    
    for(int i = 0; i < PAGE_ENTRIES; i++){
        kernel_page_dir[i] = 0;
    }
    
    for(uint32_t addr = 0; addr < 0x800000; addr += PAGE_SIZE){
        map_page(kernel_page_dir, addr, addr, PAGE_RW);
    }
    
    INFO("Activating paging...\n");
    /** 
     * Activate paging
     */
    EnablePaging(kernel_page_dir_phys);
}