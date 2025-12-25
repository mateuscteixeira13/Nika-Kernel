#include <pic.h>
#include <irq.h>
#include <hal.h>
#include <stdbool.h>

/**
 * Why not add mouse? It's for User Experience °O°  
*/

#define MOUSE_DATA_PORT 0x60
#define MOUSE_STATUS_PORT 0x64
#define MOUSE_CMD_ENABLE 0xA8
#define MOUSE_CMD_WRITE  0xD4
#define MOUSE_RESET      0xFF
#define MOUSE_ENABLE_DATA 0xF4

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768

static uint8_t mouse_packet[4];
static uint8_t packet_index = 0;


static int mouse_x = SCREEN_WIDTH / 2;
static int mouse_y = SCREEN_HEIGHT / 2;
static bool left = false;
static bool right = false;
static bool middle = false;
static int scroll = 0;

static void wait_for_input() {
    while (x86read(MOUSE_STATUS_PORT) & 0x2) {
        Halt(); // Preserve CPU energy!
    } 
}

static void send_mouse_command(uint8_t cmd) {
    wait_for_input();
    x86write(MOUSE_CMD_WRITE, MOUSE_STATUS_PORT);
    wait_for_input();
    x86write(cmd, MOUSE_DATA_PORT);
}

static void process_packet() {
    int8_t dx = (int8_t)mouse_packet[1];
    int8_t dy = (int8_t)mouse_packet[2];
    int8_t ds = 0;
    if (packet_index == 4) ds = (int8_t)mouse_packet[3];

    mouse_x += dx;
    mouse_y -= dy;

    if (mouse_x < 0) mouse_x = 0;
    if (mouse_y < 0) mouse_y = 0;
    if (mouse_x > SCREEN_WIDTH - 1) mouse_x = SCREEN_WIDTH - 1;
    if (mouse_y > SCREEN_HEIGHT - 1) mouse_y = SCREEN_HEIGHT - 1;

    left   = mouse_packet[0] & 0x1;
    right  = mouse_packet[0] & 0x2;
    middle = mouse_packet[0] & 0x4;
    scroll = ds;
}

void mouse_irq_handler() {
    uint8_t data = x86read(MOUSE_DATA_PORT);
    mouse_packet[packet_index++] = data;

    if (packet_index == 3 || packet_index == 4) {
        process_packet();
        packet_index = 0;
    }

    pic_send_eoi(12);
}

void mouse_init() {
    send_mouse_command(MOUSE_CMD_ENABLE);  
    send_mouse_command(MOUSE_RESET);     
    send_mouse_command(MOUSE_ENABLE_DATA); 
    irq_install_handler(12, mouse_irq_handler);
}

int mouse_get_x() { 
    return mouse_x; 
}

int mouse_get_y() { 
    return mouse_y; 
}

bool mouse_left_pressed() {
     return left; 
}

bool mouse_right_pressed() { 
    return right; 
}

bool mouse_middle_pressed() {
     return middle; 
}

int mouse_get_scroll() {
     return scroll; 
}
