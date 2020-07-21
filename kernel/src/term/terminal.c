#include "terminal.h"

static terminal_callbacks_t term_impl;

void init_terminal(terminal_callbacks_t callbacks)
{
    term_impl = callbacks;
}

void putc(char c)
{
    term_impl.putc(c);
}

void puts(const char* str)
{
    term_impl.puts(str);
}

void printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char buffer[1024];
    stbsp_vsprintf(buffer, fmt, args);
    puts(buffer);

    va_end(args);
}

void set_color(terminal_color_t fg, terminal_color_t bg)
{
    term_impl.set_color(fg, bg);
}
