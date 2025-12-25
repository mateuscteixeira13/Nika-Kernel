#include <hal.h>
#include <vga.h>
#include <stdint.h>
#include <pic.h>
#include <irq.h>

// CPU Operations
void x86Cli(){
    __asm__ volatile("cli");
}

void x86Sti(){
    __asm__ volatile("sti");
}

void Halt(){
    __asm__ volatile("hlt");
}

// Port x86 operations
void x86write(uint16_t port, uint8_t val){
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

uint8_t x86read(uint16_t port){
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void x86IO_wait(){
    __asm__ volatile("outb %%al, $0x80" :: "a"(0));
}

/** 
 * I used OSDev to reference
 */
void x86IRQ_setmask(uint32_t irq_line){
    if(irq_line > 15){
        return;
    }

    uint16_t port;
    uint8_t value;
    
    if(irq_line < 8){
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_line -= 8;
    }
    value = x86read(port) | (1 << irq_line);
    x86write(port, value);
}

void x86IRQ_clearmask(uint32_t irq_line){
    uint16_t port;
    uint8_t value;
    
    if(irq_line < 8){
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_line -= 8;
    }
    value = x86read(port) & ~(1 << irq_line);
    x86write(port, value);
}

/**
 * Initalize the Hardware Abstraction Layer
 */
HAL_Status Hal_init(){
    // TODO: This will be implemented, for now only return SUCCESS
    return HAL_SUCESS;
}