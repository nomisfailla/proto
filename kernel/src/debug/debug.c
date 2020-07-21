#include "debug.h"

void kdebug_c(char c)
{
    // Write to serial for QEMU.
    serial_write(c);

    // Write to port E9 for Bochs.
    out8(0xe9, c);
}

void kdebug_s(const char* buf)
{
    while(*buf)
	{
		kdebug_c(*buf++);
	}
}

void kdebug(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char buffer[1024];
    stbsp_vsprintf(buffer, fmt, args);
    kdebug_s(buffer);

    va_end(args);
}
