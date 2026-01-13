#pragma once

#include <stddef.h>
#include <stdint.h>

int strcmp(const char *a, const char *b);
char *strncpy(char *dest, const char *src, size_t n);
char* strcpy(char* dest, const char* src);
char* strchr(const char* str, char c);
size_t strlen(const char* str);
