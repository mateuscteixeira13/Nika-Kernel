#ifndef HAL_H
#define HAL_H

#include <stdint.h>
#include <multiboot.h>

typedef enum {
    HAL_SUCESS,
    HAL_ERROR
} HAL_Status;

// CPU Operations
void x86Cli();
void x86Sti();
void CPU_halt();

/**
 * HAL initialization
 */
HAL_Status Hal_init(multiboot_info_t* mbd, uint32_t magic);

#endif