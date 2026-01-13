#pragma once

#include <stdint.h>

// Just here this code do nothing

// TODO: HEY! Implement me!
void __hiuser();
void enter_usermode(uint32_t code_addr, uint32_t code_size, uint32_t stack_top);
