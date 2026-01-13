#pragma once

void  __serial();
void serial_write_byte(char c);
void serial_write(const char* str);
void serial_printf(const char* fmt, ...);
