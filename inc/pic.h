#pragma once

#include <stdint.h>
/**
 * PIC Constants
 */
#define PIC1 0x20
#define PIC2 0xA0
#define PIC1_CMD PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_CMD PIC2
#define PIC2_DATA (PIC2 + 1)

#define PIC_EOI 0x20

void pic_remap(int offset1, int offset2);
void pic_disable();
void pic_send_eoi(uint8_t irq);
void pic_init();