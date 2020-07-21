#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdarg.h>

#include "../hal/serial.h"
#include "../lib/sprintf.h"

void kdebug_c(char c);
void kdebug_s(const char* buf);
void kdebug(const char* fmt, ...);

#endif
