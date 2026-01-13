#pragma once

#include <stdint.h>
#include <multiboot.h>

typedef enum {
    HAL_SUCESS,
    HAL_ERROR
} HAL_Status;

// CPU Operations
void x86Cli();
void x86Sti();
void Halt();

// Port x86 Operations
void x86write(uint16_t port, uint8_t val);
uint8_t x86read(uint16_t port);
void x86WriteL(uint16_t port, uint32_t value);
uint32_t x86ReadL(uint16_t port);
void x86IO_wait();

// IO Operations
void x86IRQ_setmask(uint32_t irq_line);      
void x86IRQ_clearmask(uint32_t irq_line);     

void EnablePaging(uint32_t addr);
