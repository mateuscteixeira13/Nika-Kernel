#include <multiboot.h>
#include <stdint.h>
#include <vga.h>
#include <gd.h>
#include <pit.h>
#include <serial.h>
#include <paging.h>
#include <font8x8_basic.h>
#include <heap.h>
#include <stdarg.h>


// GD = Graphical Driver
int gd_ok = 0;

typedef struct gd_framebuffer{
    uintptr_t addr;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint8_t bpp;
    uint8_t type;
} gd_framebuffer_t;

static gd_framebuffer_t fb_info;

typedef struct {
    uint32_t color;      
    uint32_t x, y;       
} gdl_log_t;

static gdl_log_t gdl_log = {0xFFFFFF, 0, 0};


GD_STATUS get_framebuffer(multiboot_info_t* mbd, uint32_t magic){
    
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC){
        EarlyLog("GD: Error! The magic is invalid! Got: 0x%x\n", magic);
        return GERROR;
    }
    
    if(!(mbd->flags & (1 << 12))){
        EarlyLog("GD: Error! No framebuffer (flags: 0x%x)\n", mbd->flags);
        return GERROR;
    }
    

    fb_info.type = mbd->framebuffer_type;

   
    if(fb_info.type != 1 && fb_info.type != 2){
        EarlyLog("GD: Error! Framebuffer type is not RGB (type=%d)\n", fb_info.type);
        return GERROR;
    }
    
    fb_info.addr = (uintptr_t)mbd->framebuffer_addr;
    fb_info.width = mbd->framebuffer_width;
    fb_info.height = mbd->framebuffer_height;
    fb_info.pitch = mbd->framebuffer_pitch;
    fb_info.bpp = mbd->framebuffer_bpp;
   
    
    if(fb_info.width == 0 || fb_info.height == 0){
        EarlyLog("GD: Error! Invalid dimensions\n");
        return GERROR;
    }
    
    if(fb_info.bpp != 32 && fb_info.bpp != 24 && fb_info.bpp != 16){
        EarlyLog("GD: Warning! BPP is %d (expected 16, 24 or 32)\n", fb_info.bpp);
    }
    
    if(fb_info.addr == 0){
        EarlyLog("GD: Error! Framebuffer address is NULL\n");
        return GERROR;
    }
    
    return GSUCCESS;
}

GD_STATUS gd_init(multiboot_info_t* mbd, uint32_t magic){
    if(get_framebuffer(mbd, magic) != 0){
        EarlyLog("GD Failed to get framebuffer\n");
        return GERROR;
    }

    uint32_t fb_size = fb_info.height * fb_info.pitch;
    map_framebuffer(fb_info.addr, fb_size);

    gd_ok = 1;
    
    return GSUCCESS;
}

static inline void write_fb_pixel(volatile uint32_t* fb, uint32_t offset, uint32_t color) {
    fb[offset] = color;
}

void put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if(!gd_ok) return;
    if(x >= fb_info.width || y >= fb_info.height) return;

    if(fb_info.bpp == 32){
        volatile uint32_t* fb = (volatile uint32_t*)fb_info.addr;
        uint32_t offset = y * (fb_info.pitch / 4) + x;
        fb[offset] = color;
    } else if(fb_info.bpp == 24){
        volatile uint8_t* fb = (volatile uint8_t*)fb_info.addr;
        uint32_t offset = y * fb_info.pitch + x * 3;
        fb[offset + 0] = (color >> 0) & 0xFF;  
        fb[offset + 1] = (color >> 8) & 0xFF;  
        fb[offset + 2] = (color >> 16) & 0xFF; 
    } else if(fb_info.bpp == 16){
        volatile uint16_t* fb = (volatile uint16_t*)fb_info.addr;
        uint32_t offset = y * (fb_info.pitch / 2) + x;

        // Convert 32-bit RGB (0xRRGGBB) to RGB565 Supporting 16 bpp mode!
        uint16_t r = (color >> 19) & 0x1F; // 5 bits
        uint16_t g = (color >> 10) & 0x3F; // 6 bits
        uint16_t b = (color >> 3) & 0x1F;  

        fb[offset] = (r << 11) | (g << 5) | b;
    }
}

void draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color){
    if(gd_ok == 0){
        EarlyLog("GD is not initialized\n");
        return;
    }
    
    
    for(uint32_t i = 0; i < h; i++){
        for(uint32_t j = 0; j < w; j++){
            put_pixel(x + j, y + i, color);
        }
    }
}

void clear(uint32_t color){
    if(gd_ok == 0){
        EarlyLog("GD is not initialized\n");
        return;
    }
    
    draw_rect(0, 0, fb_info.width, fb_info.height, color);
}

void scroll_screen(uint32_t pixels){
    uint8_t* fb = (uint8_t*)fb_info.addr;
    uint32_t bytes_per_line = fb_info.pitch * (fb_info.bpp / 8);

    memmove(fb, fb + pixels * bytes_per_line, (fb_info.height - pixels) * bytes_per_line);
    draw_rect(0, fb_info.height - pixels, fb_info.width, pixels, 0x000000);
}

void draw_char(uint32_t x, uint32_t y, unsigned char c, uint32_t color){
    if(c > 127) return;
    for(int row = 0; row < 8; row++){
        uint8_t bits = font8x8_basic[(int)c][row];
        for(int col = 0; col < 8; col++){
            if(bits & (1 << col)){
                put_pixel(x + col, y + row, color);
            }
        }
    }
}

void gdl_putchar(char c) {
    if(c == '\n') {
        gdl_log.x = 0;
        gdl_log.y += 8;
        if(gdl_log.y + 8 > fb_info.height) {
            scroll_screen(8);
            gdl_log.y -= 8;
        }
    } else {
        draw_char(gdl_log.x, gdl_log.y, c, gdl_log.color);
        gdl_log.x += 8;
        if(gdl_log.x + 8 > fb_info.width) {
            gdl_log.x = 0;
            gdl_log.y += 8;
            if(gdl_log.y + 8 > fb_info.height) {
                scroll_screen(8);
                gdl_log.y -= 8;
            }
        }
    }
}

void kDebug(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char buf[32];

    while(*fmt) {
        if(*fmt == '%') {
            fmt++;
            switch(*fmt) {
                case 'd': {
                    int val = va_arg(args, int);
                    itoa(val, buf, 10);
                    for(char *p = buf; *p; p++)
                        gdl_putchar(*p);
                    break;
                }
                case 'x': {
                    int val = va_arg(args, int);
                    itoa(val, buf, 16);
                    for(char *p = buf; *p; p++)
                        gdl_putchar(*p);
                    break;
                }
                case 'p': {
                    uintptr_t val = (uintptr_t)va_arg(args, void*);
                    gdl_print_color("0x", gdl_log.color);
                    itoa(val, buf, 16);
                    for(char *p = buf; *p; p++)
                        gdl_putchar(*p);
                    break;
                }
                case 'c': {
                    char val = (char)va_arg(args, int);
                    gdl_putchar(val);
                    break;
                }
                case 's': {
                    char *val = va_arg(args, char*);
                    while(*val) gdl_putchar(*val++);
                    break;
                }
                case '%': {
                    gdl_putchar('%');
                    break;
                }
                default:
                    gdl_putchar('%');
                    gdl_putchar(*fmt);
                    break;
            }
        } else if(*fmt == '\n') {
            gdl_log.x = 0;
            gdl_log.y += 8;
            if(gdl_log.y + 8 > fb_info.height) {
                scroll_screen(8);
                gdl_log.y -= 8;
            }
        } else {
            gdl_putchar(*fmt);
            gdl_log.x += 8;
            if(gdl_log.x + 8 > fb_info.width) {
                gdl_log.x = 0;
                gdl_log.y += 8;
            }
        }
        fmt++;
    }

    va_end(args);
}

void gdl_print_color(const char *str, uint32_t color){
    while(*str){
        if(*str == '\n'){
            gdl_log.x = 0;
            gdl_log.y += 8;
            if(gdl_log.y + 8 > fb_info.height) {
                scroll_screen(8);  // scroll up
                gdl_log.y -= 8;
            }
        } else {
            draw_char(gdl_log.x, gdl_log.y, *str, color);
            gdl_log.x += 8;
            if(gdl_log.x + 8 > fb_info.width){
                gdl_log.x = 0;
                gdl_log.y += 8;
            }
        }
        str++;
    }
}

