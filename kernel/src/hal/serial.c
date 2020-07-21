#include "serial.h"

static bool is_transmit_empty()
{
    return in8(COM1 + 5) & 0x20;
}

static bool is_serial_received()
{
    return in8(COM1 + 5) & 0x01;
}

void init_serial()
{
    out8(COM1 + 1, 0x00);    // Disable all interrupts
    out8(COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    out8(COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    out8(COM1 + 1, 0x00);    //                  (hi byte)
    out8(COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
    out8(COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    out8(COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

void serial_write(char c)
{
    while(!is_transmit_empty());
    out8(COM1, c);
}

char serial_read()
{
    while(!is_serial_received());
    return in8(COM1);
}
