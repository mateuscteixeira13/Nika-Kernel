#pragma once

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
