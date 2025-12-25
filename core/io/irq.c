#include <stdint.h>
#include <stddef.h>
#include <pic.h>
#include <irq.h>
#include <idt.h>
#include <hal.h>

extern void page_fault_stub();
/**
 * I hope this will don't have a Triple Fault :) 
 */
static irq_handler_t irq_handlers[16] = {0};

void irq_install_handler(int irq, irq_handler_t handler){
    if(irq >= 0 && irq < 16){
        irq_handlers[irq] = handler;
    }
}

void irq_uninstall_handler(int irq){
    if(irq >= 0 && irq < 16){
        irq_handlers[irq] = NULL;
    }
}

void irq_handler(registers_t *r){
    int irq_n =  r->int_no - 32; // This calculate the PIC irq(0-15)
    
    if(irq_handlers[irq_n] != NULL){
        irq_handler_t handler = irq_handlers[irq_n];
        handler(r);
    }

    // EOI
    if(irq_n >= 8){
        x86write(PIC2_CMD, PIC_EOI);
    }

    x86write(PIC1_CMD, PIC_EOI);
}
