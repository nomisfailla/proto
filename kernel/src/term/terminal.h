#ifndef _TERMINAL_H
#define _TERMINAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include "../lib/sprintf.h"

typedef enum
{
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
} terminal_color_t;

typedef void(*putc_t)(char);
typedef void(*puts_t)(const char*);
typedef void(*set_color_t)(terminal_color_t, terminal_color_t);

typedef struct
{
    putc_t putc;
    puts_t puts;
    set_color_t set_color;
} terminal_callbacks_t;

void init_terminal(terminal_callbacks_t impl);

void putc(char c);
void puts(const char* str);
void printf(const char* fmt, ...);
void set_color(terminal_color_t fg, terminal_color_t bg);

#endif
