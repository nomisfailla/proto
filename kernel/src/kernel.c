#include <stdint.h>

#include "boot/multiboot_parser.h"

#include "cpu/gdt.h"
#include "cpu/idt.h"

#include "mem/kmem.h"
#include "mem/paging.h"

#include "hal/serial.h"

#include "term/terminal.h"
#include "vga/vga_textmode.h"
#include "vga/vga_gfxmode.h"

#include "debug/debug.h"

void kmain(uint32_t mbootptr, uint32_t magic)
{
	init_serial();

	multiboot_capabilities_t caps = parse_multiboot(mbootptr, magic);
	if(caps.framebuffer.type == FB_TEXT)
	{
		terminal_callbacks_t impl = init_vga_textmode();
		init_terminal(impl);
	}
	else
	{
		terminal_callbacks_t impl = init_vga_gfxmode(caps.framebuffer.address, caps.framebuffer.pitch);
		init_terminal(impl);
	}

	init_gdt();
	init_idt();

	init_paging();

    puts("Welcome to ");
    set_color(COLOR_CYAN, COLOR_BLACK);
    puts("Proto ");
    set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
    puts("v0.1");
    set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    puts("!\n");

	puts("Running in ");
	set_color(COLOR_GREEN, COLOR_BLACK);
	if(caps.framebuffer.type == FB_TEXT) puts("text mode");
	if(caps.framebuffer.type == FB_PIXELS) puts("gfx mode");
    set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
	puts(" at ");
    set_color(COLOR_LIGHT_MAGENTA, COLOR_BLACK);
	printf("%d", caps.framebuffer.width);
    set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
	printf("x");
    set_color(COLOR_LIGHT_MAGENTA, COLOR_BLACK);
	printf("%d", caps.framebuffer.height);
    set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
	printf("x");
    set_color(COLOR_LIGHT_MAGENTA, COLOR_BLACK);
	printf("%d", caps.framebuffer.bpp);
    set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
	puts(".\n");

	puts("Color test;\n");
	for(int y = 0; y < 16; y++)
	{
		set_color(COLOR_BLACK, y);
		putc(' ');
	}
	putc('\n');
	for(int y = 0; y < 16; y++)
	{
		set_color(y, COLOR_BLACK);
		putc('@');
	}
	putc('\n');
    set_color(COLOR_LIGHT_GREY, COLOR_BLACK);

	// Enable interrupts once initialization is done.
    asm volatile("sti");

	//uint32_t *ptr = (uint32_t*)0xA0000000;
	//uint32_t do_page_fault = *ptr;
	//printf("%d\n", do_page_fault);

	for(;;);
}
