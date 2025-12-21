#include <stdint.h>
#include <stddef.h>
#include <fork.h>
#include <heap.h>
#include <paging.h>
#include <mem.h>
#include <lib.h>

/** 
 * It's simple but functional :)
 */

process_t* current_process;
process_t* process_list;
uint32_t next_pid = 1;

process_t* create_process(uint32_t eip, __attribute__((unused))uint32_t esp,  __attribute__((unused))uint32_t ebp){
    process_t *p = (process_t*) malloc(sizeof(process_t));
    p->pid = next_pid++;
    p->eip = eip;
    p->page_dir_phys = (uint32_t) pmm_alloc_page();
    p->page_dir = (uint32_t*)p->page_dir_phys;
    memset(p->page_dir, 0, PAGE_SIZE);
    clone_kernel_space(p->page_dir);
    
    uint32_t stack_phys = (uint32_t)pmm_alloc_page();
    map_page(p->page_dir, 0xBFF000, stack_phys, PAGE_RW | PAGE_USER | PAGE_PRESENT);
    memset((void*)stack_phys, 0, PAGE_SIZE);
    
    p->esp = 0xBFF000 + PAGE_SIZE - 16;
    p->ebp = p->esp;
    p->eflags = 0x202;
    p->status = READY;
    p->next = process_list;
    process_list = p;
    return p;
}

void switch_to_process(process_t *next){
    if(next == NULL) return;
    current_process = next;
    __asm__ volatile(
        "mov %0, %%cr3\n"
        "mov %%cr3, %%eax\n"
        "mov %%eax, %%cr3"
        :
        : "r"(current_process->page_dir_phys)
        : "eax"
    );
}

void schedule(){
    if(process_list == NULL) return;
    
    if(current_process == NULL){
        current_process = process_list;
    } else {
        current_process = current_process->next ? current_process->next : process_list;
    }
    switch_to_process(current_process);
}

int __fork(){
    if(current_process == NULL) return -1;
    
    process_t* parent = current_process;
    process_t* child = create_process(parent->eip, parent->esp, parent->ebp);
    
    if(child == NULL) return -1;
    
    copy_addr_space(parent, child);
    
    return (current_process == parent) ? child->pid : 0;
}