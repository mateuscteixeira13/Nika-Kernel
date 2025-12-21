#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>

typedef struct {
    uint32_t ds;
    uint32_t edi, esi, edp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} registers_t; // CPU x86 registers

typedef void (*irq_handler_t)(registers_t*); // Function Type


// Declarations
void irq_install_handler(int irq, irq_handler_t handler);
void irq_uninstall_handler(int irq);
void irq_handler(registers_t *r);
void init_io();
void irq_install();

/**
 * Bro, 15 irq .-. Why? lol
 */
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

#endif