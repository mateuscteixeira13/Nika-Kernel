#include <stdint.h>
#include <idt.h>

/**
 * I hope it works Lol
 */

#define IDT_ENTRIES 256

/**
 * IDT struct
 */
typedef struct{
    uint16_t base_low;
    uint16_t sel;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
}__attribute__((packed)) idt_entry_t;

/**
 * IDT Pointer
 */
typedef struct{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

idt_entry_t idt[IDT_ENTRIES];
idt_ptr_t idt_ptr;

extern void idt_load(uint32_t);

void idt_set_gate(uint8_t n, uint32_t handler, uint16_t sel, uint8_t flags){
    idt[n].base_low = handler & 0xFFFF;
    idt[n].base_high = (handler >> 16) & 0xFFFF;

    idt[n].sel = sel; 
    idt[n].zero = 0; // Obviously zero LOL :P
    idt[n].flags = flags;
}

/**
 * Finally I arrived here!
 */
void idt_init(){
    idt_ptr.limit = sizeof(idt_entry_t) * IDT_ENTRIES - 1;
    idt_ptr.base = (uint32_t)&idt;
    
    for(int i = 0; i < IDT_ENTRIES; i++){
        idt[i].base_low = 0;
        idt[i].base_high = 0;
        idt[i].sel = 0;
        idt[i].flags = 0;
    }

    idt_load((uint32_t)&idt_ptr);
}

/**
 * Triple Fault ._. But I correct this, this works! No Triple Fault
 */