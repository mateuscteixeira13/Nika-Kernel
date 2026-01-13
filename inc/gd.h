#pragma once

#include <stdint.h>
#include <multiboot.h>

typedef enum{
    GSUCCESS,
    GERROR
} GD_STATUS;

void put_pixel(uint32_t x, uint32_t y, uint32_t color);
void draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void clear(uint32_t color);
GD_STATUS gd_init(multiboot_info_t* mbd, uint32_t magic);
void kDebug(const char *fmt, ...);
void gdl_putchar(char c);
void gdl_print_color(const char *str, uint32_t color);

/**
 * Colors for VBE 32/24
 */

#define BLACK        0x000000
#define WHITE        0xFFFFFF

#define RED          0xFF0000
#define GREEN        0x00FF00
#define BLUE         0x0000FF

#define YELLOW       0xFFFF00
#define CYAN         0x00FFFF
#define MAGENTA      0xFF00FF

#define DARK_GRAY    0x404040
#define GRAY         0x808080
#define LIGHT_GRAY   0xC0C0C0

#define ORANGE       0xFFA500
#define BROWN        0x8B4513
#define PINK         0xFFC0CB

#define SKY_BLUE     0x87CEEB
#define LIME         0x32CD32
#define PURPLE       0x800080
