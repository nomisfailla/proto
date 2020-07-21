#include "vga_gfxmode.h"
#include "font.h"

static uint32_t gfx_framebuffer;
static size_t gfx_pitch;

static size_t gfx_offset;

static size_t gfx_row;
static size_t gfx_col;

static terminal_color_t gfx_fg;
static terminal_color_t gfx_bg;

static uint32_t colormap[16] = {
    0x000000,
    0x0000AA,
    0x00AA00,
    0x00AAAA,
    0xAA0000,
    0xAA00AA,
    0xAA5500,
    0xAAAAAA,
    0x555555,
    0x5555FF,
    0x55FF55,
    0x55FFFF,
    0xFF5555,
    0xFF55FF,
    0xFFFF55,
    0xFFFFFF
};

static void draw_char(uint8_t *where, uint32_t character, uint32_t foreground_colour, uint32_t background_colour)
{
    uint8_t* font_data_for_char = &system_font_data_address[character * 14];

    for (int row = 0; row < 14; row++)
	{
        uint8_t row_data = font_data_for_char[row];
		for(int col = 0; col < 8; col++)
		{
        	*(uint32_t*)(&where[col * 3]) = (row_data & (0x80 >> col)) ? foreground_colour : background_colour;
		}

        where += gfx_pitch;
    }
}

static void vga_gfxmode_putc(char c)
{
	if (c == '\n')
	{
		gfx_row++;
		gfx_col = 0;
	}
	else
	{
        uint32_t offset = (gfx_row * gfx_pitch * 14) + gfx_col * 8 * 3;

        draw_char((uint8_t*)(gfx_framebuffer + offset), c, colormap[gfx_fg], colormap[gfx_bg]);
        //offset += (3 * 8);

		//put_char_at(c, vga_textmode_color, vga_textmode_column, vga_textmode_row);

        gfx_col++;
		//if (++vga_textmode_column == VGA_WIDTH)
		//{
		//	vga_textmode_column = 0;
		//	vga_textmode_row++;
		//}
	}

	//if (vga_textmode_row == VGA_HEIGHT)
	//{
	//	vga_textmode_scroll();
	//}
}

static void vga_gfxmode_puts(const char* str)
{
    while(*str)
	{
		vga_gfxmode_putc(*str++);
	}
}

static void vga_gfxmode_set_color(terminal_color_t fg, terminal_color_t bg)
{
    gfx_fg = fg;
    gfx_bg = bg;
}

terminal_callbacks_t init_vga_gfxmode(uint32_t framebuffer, size_t pitch)
{
    gfx_framebuffer = framebuffer;
    gfx_pitch = pitch;

    gfx_row = 0;
    gfx_col = 0;
    gfx_offset = 0;

	vga_gfxmode_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);

    terminal_callbacks_t impl;
    impl.putc = vga_gfxmode_putc;
    impl.puts = vga_gfxmode_puts;
    impl.set_color = vga_gfxmode_set_color;
    return impl;
}
