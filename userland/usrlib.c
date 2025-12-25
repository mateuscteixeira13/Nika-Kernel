#include <vbe.h>

int write(int fd, const char* buf, int len) {
    int ret;
    asm volatile(
        "mov $1, %%eax\n"
        "mov %1, %%ebx\n"
        "mov %2, %%ecx\n"
        "mov %3, %%edx\n"
        "int $0x80\n"
        "mov %%eax, %0\n"
        : "=r"(ret)
        : "r"(fd), "r"(buf), "r"(len)
        : "eax","ebx","ecx","edx"
    );
    return ret;
}

int exit(int code) {
    asm volatile(
        "mov $0, %%eax\n"
        "mov %0, %%ebx\n"
        "int $0x80\n"
        :
        : "r"(code)
        : "eax","ebx"
    );
    while(1);
}

