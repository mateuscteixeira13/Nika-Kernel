#pragma once

#include <gd.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

void InitStdio();
void Stdout_color(const char *str, unsigned int color);
void Stdout(const char *str);
void StdioClean_color();
void StdioClean();
int EarlyLog(const char *fmt, ...);
void putchar(char c);
void Stdioback(void);
void itoa(int val, char *buf, int base);

#define panic(str) Stdout(str)
#define INFO(str) kDebug("[INFO] %s", str);
#define WARN(str) EarlyLog("[WARN] ", str);
