#include "vga_textmode.h"

static size_t vga_textmode_row;
static size_t vga_textmode_column;

static terminal_color_t vga_textmode_bg;
static terminal_color_t vga_textmode_fg;

static vga_text_cell_t* vga_textmode_buffer;

static void put_char_at(char c, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	vga_textmode_buffer[index].chr = c;
	vga_textmode_buffer[index].fg = vga_textmode_fg;
	vga_textmode_buffer[index].bg = vga_textmode_bg;
}

static void update_cursor()
{
	const size_t cur_pos = vga_textmode_row * 80 + vga_textmode_column;
	out8(0x3D4, 14);
	out8(0x3D5, cur_pos >> 8);
	out8(0x3D4, 15);
	out8(0x3D5, cur_pos);
}

static void vga_textmode_putc(char c)
{
	if (c == '\n')
	{
		vga_textmode_row++;
		vga_textmode_column = 0;
	}
	else
	{
		put_char_at(c, vga_textmode_column, vga_textmode_row);

		if (++vga_textmode_column == VGA_WIDTH)
		{
			vga_textmode_column = 0;
			vga_textmode_row++;
		}
	}

	if (vga_textmode_row == VGA_HEIGHT)
	{
		vga_textmode_scroll();
	}

	update_cursor();
}

static void vga_textmode_puts(const char* str)
{
	while(*str)
	{
		vga_textmode_putc(*str++);
	}
}

static void vga_textmode_set_color(terminal_color_t fg, terminal_color_t bg)
{
	vga_textmode_fg = fg;
	vga_textmode_bg = bg;
}

terminal_callbacks_t init_vga_textmode()
{
	vga_textmode_buffer = (vga_text_cell_t*) 0xB8000;
	vga_textmode_clear();

	terminal_callbacks_t impl;
	impl.putc = vga_textmode_putc;
	impl.puts = vga_textmode_puts;
	impl.set_color = vga_textmode_set_color;
	return impl;
}

void vga_textmode_clear()
{
	vga_textmode_row = 0;
	vga_textmode_column = 0;
	vga_textmode_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);

	for (size_t y = 0; y < VGA_HEIGHT; y++)
	{
		for (size_t x = 0; x < VGA_WIDTH; x++)
		{
			const size_t index = y * VGA_WIDTH + x;
			vga_textmode_buffer[index].chr = ' ';
			vga_textmode_buffer[index].fg = vga_textmode_fg;
			vga_textmode_buffer[index].bg = vga_textmode_bg;
		}
	}
	
	update_cursor();
}

void vga_textmode_scroll()
{
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			const size_t nextrow_index = index + VGA_WIDTH;

			if (y == VGA_HEIGHT) //Last line, make it blank
			{
				vga_textmode_buffer[index].chr = ' ';
				vga_textmode_buffer[index].fg = vga_textmode_fg;
				vga_textmode_buffer[index].bg = vga_textmode_bg;
			}
			else
			{
				vga_textmode_buffer[index] = vga_textmode_buffer[nextrow_index];
			}
        }
    }
    vga_textmode_row = VGA_HEIGHT-1;
    vga_textmode_column = 0;
}
