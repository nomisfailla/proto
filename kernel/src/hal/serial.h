#ifndef _SERIAL_H
#define _SERIAL_H

#include <stdbool.h>

#include "../cpu/ports.h"

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

void init_serial();

void serial_write(char c);
char serial_read();

#endif
