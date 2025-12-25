#ifndef SYSCALL_H
#define SYSCALL_H

void __hiuser();
void enter_usermode(uint32_t code_addr, uint32_t code_size, uint32_t stack_top);

#endif