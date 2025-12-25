#include <stdint.h>

extern int write(int fd, const char* buf, int len);
extern int exit(int code);

void user_main() {
    const char* msg = "Hi kernel! I am user!\n";
    write(1, msg, 28);  
}