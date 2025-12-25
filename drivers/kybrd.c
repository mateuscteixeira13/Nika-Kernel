#include <stdint.h>
#include <irq.h>
#include <vga.h>
#include <hal.h>
#include <pic.h>
#include <stddef.h>
#include <serial.h>

/**
 * A basic keyboard driver
 */
#define KBD_DATAPORT 0x60

static const char ky_scancodes[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,   'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,  '\\','z','x','c','v','b','n','m',',','.','/',
    0,   '*', 0,  ' ',
};

void kybrd_handler(registers_t *r){
    (void)r;

    uint8_t scancode = x86read(KBD_DATAPORT);


    if (scancode & 0x80){
        return;
    }

    if (scancode >= 128){
        return;
    }

    char k = ky_scancodes[scancode];
    if (!k){
        return;
    }

    if(k == '\n'){
        return;
    }
   
    if (k == '\b') {
        return;
    }

    if (k == '\t' || k == 27){
        return;
    }

}

void kybrd_init(){
    x86IRQ_clearmask(1);
    irq_install_handler(1, kybrd_handler);
}