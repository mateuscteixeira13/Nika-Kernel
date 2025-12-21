#include <stdint.h>
#include <stddef.h>
#include <pic.h>
#include <irq.h>
#include <idt.h>
#include <asm.h> // Include the asm functions for GCC stop to warning :)


extern void page_fault_stub();
/**
 * I hope this will don't have a Triple Fault :) 
 */
static irq_handler_t irq_handlers[16] = {0};

/** 
 * I used OSDev to reference
 */
void irq_set_mask(uint32_t irq_line){
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
    value = inb(port) | (1 << irq_line);
    outb(port, value);
}

void irq_clear_mask(uint32_t irq_line){
    uint16_t port;
    uint8_t value;
    
    if(irq_line < 8){
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_line -= 8;
    }
    value = inb(port) & ~(1 << irq_line);
    outb(port, value);
}


void irq_install(){
    idt_set_gate(14, (uint32_t)page_fault_stub, 0x08, 0x8E);
    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);
    idt_set_gate(34, (uint32_t)irq2, 0x08, 0x8E);
    idt_set_gate(35, (uint32_t)irq3, 0x08, 0x8E);
    idt_set_gate(36, (uint32_t)irq4, 0x08, 0x8E);
    idt_set_gate(37, (uint32_t)irq5, 0x08, 0x8E);
    idt_set_gate(38, (uint32_t)irq6, 0x08, 0x8E);
    idt_set_gate(39, (uint32_t)irq7, 0x08, 0x8E);
    idt_set_gate(40, (uint32_t)irq8, 0x08, 0x8E);
    idt_set_gate(41, (uint32_t)irq9, 0x08, 0x8E);
    idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E);
    idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
    idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
    idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E);
    idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);
    /**
     * Finally this finished 
     */
}

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
        outb(PIC2_CMD, PIC_EOI);
    }

    outb(PIC1_CMD, PIC_EOI);
}

void init_io(){
    irq_install();
}