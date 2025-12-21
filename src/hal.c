#include <hal.h>
#include <vga.h>
#include <mem.h>
#include <stdint.h>
#include <multiboot.h>
#include <pic.h>
#include <idt.h>
#include <irq.h>
#include <heap.h>
#include <paging.h>

// CPU Operations
void x86Cli(){
    __asm__ volatile("cli");
}

void x86Sti(){
    __asm__ volatile("sti");
}

void CPU_halt(){
    __asm__ volatile("hlt");
}

/**
 * Initalize the Hardware Abstraction Layer
 */
HAL_Status Hal_init(multiboot_info_t* mbd, uint32_t magic){

    idt_init();
    init_io();
    pic_init();

    if(pmm_init(mbd, magic) != 0){
       Stdout("Error to initialize the PMM\n");
       return HAL_ERROR;
    }

    init_pages();

    heap_init();
    
    return HAL_SUCESS;
}