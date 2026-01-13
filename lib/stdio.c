#include <stdint.h>
#include <stdarg.h>
#include "paging.h"
#include <vga.h>

volatile uint16_t *vga;

volatile uint16_t* vga_at(int x, int y) {
    return (volatile uint16_t*)(0xB8000 + 2 * (y * VGA_WIDTH + x));
}

void InitStdio() {
    vga = vga_at(0, 0);
}

void putchar(char c) {
    static int x = 0, y = 0;
    volatile uint16_t* cell = vga_at(x, y);

    if(c == '\n') {
        x = 0;
        y++;
    } else {
        *cell = ((uint16_t)c) | (0x0F << 8); 
        x++;
        if(x >= VGA_WIDTH) {
            x = 0;
            y++;
        }
    }

    if(y >= VGA_HEIGHT) {
        y = 0;
    }
}

void Stdout(const char *s) {
    while(*s) putchar(*s++);
}

void itoa(int val, char *buf, int base) {
    char *ptr = buf;
    int sign = 0;

    if(val < 0 && base == 10) {
        sign = 1;
        val = -val;
    }

    int temp = val;
    do {
        int digit = temp % base;
        *ptr++ = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        temp /= base;
    } while(temp);

    if(sign) *ptr++ = '-';
    *ptr = '\0';

    for(char *start = buf, *end = ptr - 1; start < end; start++, end--) {
        char t = *start;
        *start = *end;
        *end = t;
    }
}

int EarlyLog(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char buf[32];

    for(; *fmt; fmt++) {
        if(*fmt != '%') {
            putchar(*fmt);
            continue;
        }
        fmt++;

        switch(*fmt) {
            case 's':
                Stdout(va_arg(args, char*));
                break;

            case 'c':
                putchar((char)va_arg(args, int));
                break;

            case 'd':
                itoa(va_arg(args, int), buf, 10);
                Stdout(buf);
                break;

            case 'x':
                itoa(va_arg(args, int), buf, 16);
                Stdout(buf);
                break;

            case 'p':
                uintptr_t ptr = (uintptr_t)va_arg(args, void*);
                putchar('0');
                putchar('x');
                itoa(ptr, buf, 16);
                Stdout(buf);
                break;

            case '%':
                putchar('%');
                break;

            default:
                putchar('?');
                break;
        }
    }

    va_end(args);
    return 0;
}
