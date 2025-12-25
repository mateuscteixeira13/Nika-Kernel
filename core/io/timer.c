#include <stdint.h>
#include <hal.h>
#include <pit.h>
#include <irq.h>
#include <vga.h>

volatile uint64_t sys_ticks = 0;

void __setup_pit(uint32_t freq){
    
    uint32_t div = 1193182 / freq;

    x86write(0x43, 0x36);

    x86write(0x40, (uint8_t)(div & 0xFF));
    x86write(0x40, (uint8_t)((div >> 8) & 0xFF));
}

void ksleep(uint32_t ms){
    uint64_t target_ticks = sys_ticks + ms;
    while (sys_ticks < target_ticks){
        __asm__ volatile("hlt");
    }
}

void timer_handler(){
    sys_ticks++;
}

void pit_init(){
    irq_install_handler(0, timer_handler);
    x86IRQ_clearmask(0);
    __setup_pit(1000);
}