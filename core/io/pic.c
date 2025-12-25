#include <stdint.h>
#include <stddef.h>
#include <pic.h>
#include <hal.h>

/**
 * OSDev
 */

/**
 * PIC Controllers
 */
#define ICW1_ICW4 0x01
#define ICW1_SINGLE 0x02
#define ICW1_INTERVAL 0x04
#define ICW1_LEVEL 0x18
#define ICW1_INIT 0x10

#define ICW4_8086 0x01
#define ICW4_AUTO 0x02
#define ICW4_BUF_SLAVE 0x08
#define ICW4_BUF_MASTER 0x0C
#define ICW4_SFNM 0x10

#define CASCADE_IRQ 2

#define PIC_READ_IRR 0x0a
#define PIC_READ_ISR 0x0b

void pic_remap(int offset1, int offset2){
    x86write(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    x86IO_wait();
    x86write(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    x86IO_wait();
    x86write(PIC1_DATA, offset1);
    x86IO_wait();
    x86write(PIC2_DATA, offset2);
    x86IO_wait();
    x86write(PIC1_DATA, 1 << CASCADE_IRQ);
    x86IO_wait();
    x86write(PIC2_DATA, 2);
    x86IO_wait();
    x86write(PIC1_DATA, ICW4_8086);
    x86IO_wait();
    x86write(PIC2_DATA, ICW4_8086);
    x86IO_wait();

    // Now this is correct! Finally :)
    x86write(PIC1_DATA, 0xFB); 
    x86write(PIC2_DATA, 0xFF);
}

void pic_disable(){
    x86write(PIC1_DATA, 0xFF);
    x86write(PIC2_DATA, 0xFF);
}

void pic_send_eoi(uint8_t irq){
    if(irq >= 8){
        x86write(PIC2_CMD, PIC_EOI);
    }

    x86write(PIC1_CMD, PIC_EOI);
}

/**
 * Helper Fuctions
 */
static uint16_t pic_get_irq_reg(int ocw3){
    x86write(PIC1_CMD, ocw3);
    x86write(PIC2_CMD, ocw3);
    return (x86read(PIC2_CMD) << 8 | x86read(PIC1_CMD));
}

uint16_t pic_get_irr(){
    return pic_get_irq_reg(PIC_READ_IRR);
}

uint16_t pic_get_isr(){
    return pic_get_irq_reg(PIC_READ_ISR);
}

void pic_init(){
    pic_remap(0x20, 0x28);
}

