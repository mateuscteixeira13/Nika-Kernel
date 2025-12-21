#include <vga.h>
#include <stddef.h>
#include <mem.h>
#include <stdarg.h>
#include <stdbool.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static unsigned int default_color = 0x07;

static volatile uint8_t* video;

void InitStdio(){
	video = (uint8_t*)0xB8000;
	StdioClean();
}

void Stdout_color(const char *str, unsigned int color){
	while (*str){
		if (*str == '\n'){
			size_t offset = video - (unsigned char *)0xB8000;
			size_t col = (offset / 2) % 80;
			video += (80 - col) * 2;
			str++;
			continue;
		}
		*video++ = *str++;
		*video++ = color;
	}
}

void Stdout(const char *str){
	Stdout_color(str, default_color);
}

static void putchar(char c){
	char str[2] = {c, 0};
	Stdout(str);
}

void StdioClean_color(unsigned int color){
	video = (unsigned char *)0xB8000;
	for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++){
		video[i * 2] = ' ';
		video[i * 2 + 1] = color;
	}
}

void StdioClean(){
	StdioClean_color(default_color);
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