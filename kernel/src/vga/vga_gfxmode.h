#ifndef _VGA_GFXMODE_H
#define _VGA_GFXMODE_H

#include <stdint.h>
#include <stddef.h>

#include "../term/terminal.h"

terminal_callbacks_t init_vga_gfxmode(uint32_t framebuffer, size_t pitch);

#endif
