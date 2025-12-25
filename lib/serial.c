#include <stdint.h>
#include <hal.h>
#include <stdarg.h>
#include <stddef.h>

#define SERIAL_PORT 0x3F8

void __serial(){
    x86write(SERIAL_PORT + 1, 0x00);
    x86write(SERIAL_PORT + 3, 0x80);
    x86write(SERIAL_PORT + 0, 0x00);
    x86write(SERIAL_PORT + 1, 0x00);
    x86write(SERIAL_PORT + 3, 0x03);
    x86write(SERIAL_PORT + 2, 0xC7);
    x86write(SERIAL_PORT + 4, 0x0B);
}

int is_serial_transmit_empty(){ // I don't know LOL hahaha
    return x86read(SERIAL_PORT + 5) & 0x20;
}

void serial_write_byte(char c){
    while (!is_serial_transmit_empty());
    x86write(SERIAL_PORT, c);
}

void serial_write(const char *str){
    for (int i = 0; str[i] != '\0'; i++){
        if (str[i] == '\n'){
            serial_write_byte('\r');
            serial_write_byte(str[i]);
        }
        serial_write_byte(str[i]); 
    }
}

static void itoa(int v, char *buf, int base){
    char temp[32];
    int i = 0, j = 0;
    int is_negative = 0;

    if (v == 0){
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }

    if (v < 0 && base == 10){
        is_negative = 1;
        v = -v;
    }

    while (v != 0){
        int rem = v % base;
        temp[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        v /= base;
    }

    if (is_negative){
        temp[i++] = '-';
    }

    for (j = 0; j < i; j++){
        buf[j] = temp[i - j - 1];
    }

    buf[i] = '\0';
}

static void ptoa(uintptr_t ptr, char *buffer){
    char temp[32];
    int i = 0, j = 0;
    if (ptr == 0){
        buffer[0] = '0';
        buffer[1] = 'x';
        buffer[2] = '0';
        buffer[3] = '\0';
        return;
    }

    while (ptr != 0){
        int rem = ptr % 16;
        temp[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        ptr /= 16;
    }

    buffer[j++] = '0';
    buffer[j++] = 'x';
    for (int k = 0; k < i; k++){
        buffer[j++] = temp[i - k - 1];
    }

    buffer[j] = '\0';
}

void serial_printf(const char *fmt, ...){
    va_list args;
    va_start(args, fmt);
    for (int i = 0; fmt[i] != '\0'; i++){
        if (fmt[i] == '%'){
            i++;
            switch (fmt[i]){
            case 'c':
                char c = (char)va_arg(args, int);
                serial_write_byte(c);
                break;
            
            case 's':
            
                const char *s = va_arg(args, const char *);
                serial_write(s);
                break;
            
            case 'd':
            
                int d = va_arg(args, int);
                char dbuf[12];
                itoa(d, dbuf, 10);
                serial_write(dbuf);
                break;
            
            case 'x':
            
                int x = va_arg(args, int);
                char xbuf[12];
                itoa(x, xbuf, 16);
                serial_write(xbuf);
                break;
            
            case 'p':
            
                void *p = va_arg(args, void *);
                char pbuf[20];
                ptoa((uintptr_t)p, pbuf);
                serial_write(pbuf);
                break;
            
            case '%':
            
                serial_write_byte('%');
                break;
            
            default:
                serial_write_byte(fmt[i]);
            }
        }

        else{
            if (fmt[i] == '\n')
                serial_write_byte('\r');
            serial_write_byte(fmt[i]);
        }
    }
    va_end(args);
}