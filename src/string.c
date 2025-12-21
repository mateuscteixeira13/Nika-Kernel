#include <stdint.h>
#include <stddef.h>
#include <lib.h>

int strcmp(const char *a, const char *b){
    while(*a && (*a == *b)){ a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}

char *strncpy(char *dest, const char *src, size_t n){
    size_t i;
    for(i = 0; i < n && src[i]; i++) dest[i] = src[i];
    for(; i < n; i++) dest[i] = 0;
    return dest;
}