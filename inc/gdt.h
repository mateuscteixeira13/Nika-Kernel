#ifndef GDT_H
#define GDT_H

void gdt_init(void);
void set_kernel_stack(uint32_t stack);

#endif