#ifndef _VGA_TEXTMODE_H
#define _VGA_TEXTMODE_H

#include <stdint.h>
#include <stddef.h>

#include "../term/terminal.h"
#include "../cpu/ports.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

typedef struct
{
	uint8_t chr;
    union
    {
        struct
        {
            uint8_t fg : 4;
            uint8_t bg : 4;
        };
        uint8_t color;
    };
} __attribute__((packed)) vga_text_cell_t;

terminal_callbacks_t init_vga_textmode();

void vga_textmode_clear();
void vga_textmode_scroll();

#endif
