#include <vga.h>
#include <stddef.h>
#include <mem.h>
#include <stdarg.h>
#include <stdbool.h>
#include <vbe.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static unsigned int default_color = 0x07;
static volatile uint8_t* video;
static size_t cursor_x = 0;
static size_t cursor_y = 0;


static inline uint8_t* vga_at(size_t x, size_t y){
    return (uint8_t*)0xB8000 + 2 * (y * VGA_WIDTH + x);
}

void InitStdio(){
    video = (uint8_t*)0xB8000;
    cursor_x = 0;
    cursor_y = 0;
    StdioClean();
}

void Stdout_color(const char *str, unsigned int color){
    unsigned int old = default_color;
    default_color = color;

    while (*str){
        putchar(*str++);
    }

    default_color = old;
}

void Stdout(const char *str){
	Stdout_color(str, default_color);
}

void putchar(char c){
    if (c == '\n'){
        cursor_x = 0;
        cursor_y++;
        return;
    }

    if (cursor_x >= VGA_WIDTH){
        cursor_x = 0;
        cursor_y++;
    }

    if (cursor_y >= VGA_HEIGHT){
        cursor_y = VGA_HEIGHT - 1;
    }

    uint8_t* cell = vga_at(cursor_x, cursor_y);
    cell[0] = c;
    cell[1] = default_color;

    cursor_x++;
}

void StdioClean_color(unsigned int color){
    for (size_t y = 0; y < VGA_HEIGHT; y++){
        for (size_t x = 0; x < VGA_WIDTH; x++){
            uint8_t* cell = vga_at(x, y);
            cell[0] = ' ';
            cell[1] = color;
        }
    }
    cursor_x = 0;
    cursor_y = 0;
}

void StdioClean(){
	StdioClean_color(default_color);
}

void Stdioback(void){
    if (cursor_x == 0 && cursor_y == 0)
        return;

    if (cursor_x == 0){
        cursor_y--;
        cursor_x = VGA_WIDTH - 1;
    } else {
        cursor_x--;
    }

    uint8_t* cell = vga_at(cursor_x, cursor_y);
    cell[0] = ' ';
    cell[1] = default_color;
}

static void itoa(int v, char *buf, int base){
	char* p = buf;
	char* p1 = buf;
	char tmp;
	int sign = 0;

	if(v < 0 && base == 10){
		sign = 1;
		v = -v;
	}

	do{
		int rem = v % base;
		*p++ = (rem < 10) ? rem + '0' : rem - 10 + 'A';
		v /= base;
	} while(v);

	if(sign){
		*p++ = '-';
	}
	
	*p-- = '\0';

	while(p1 < p){
		tmp = *p;
		*p-- = *p1;
		*p1++ = tmp;
	}
}

int atoi(const char* str) {
    int result = 0;
    int sign = 1;

    if(*str == '-') {
        sign = -1;
        str++;
    } else if(*str == '+') {
        str++;
    }

    while(*str) {
        if(*str < '0' || *str > '9') break; 
        result = result * 10 + (*str - '0');
        str++;
    }

    return result * sign;
}


int kprintf(const char *fmt, ...){
	va_list args;
	va_start(args, fmt);

	char buf[32];

	for(; *fmt; fmt++){
		if(*fmt != '%'){
			putchar(*fmt);
			continue;
		}
		fmt++;

    	switch (*fmt){
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