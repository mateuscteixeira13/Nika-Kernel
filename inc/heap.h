#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stddef.h>

void heap_init();
void* malloc(uint32_t size);
void free(__attribute__((unused)) void* ptr);
void *memset(void *dst, int val, size_t size);
void *memcpy(void *dest, const void *src, size_t n);


#endif