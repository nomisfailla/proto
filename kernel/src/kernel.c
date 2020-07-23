#include <stdint.h>

#include "boot/multiboot_parser.h"

#include "cpu/gdt.h"
#include "cpu/idt.h"

#include "mem/kmem.h"

#include "hal/serial.h"
#include "hal/pci.h"

#include "term/terminal.h"
#include "vga/vga_textmode.h"
#include "vga/vga_gfxmode.h"

#include "debug/debug.h"

void kernel_pci_enumerator(pci_device_t device)
{
    printf("%04x   %04x   %02x    %02x       ", device.vendor_id, device.device_id, device.class, device.subclass);
	switch(device.class)
    {
    case PCI_UNCLASSIFIED:                       printf("unclassified                      "); break;
    case PCI_MASS_STORAGE_CONTROLLER:            printf("mass storage controller           "); break;
    case PCI_NETWORK_CONTROLLER:                 printf("network controller                "); break;
    case PCI_DISPLAY_CONTROLLER:                 printf("display controller                "); break;
    case PCI_MULTIMEDIA_CONTROLLER:              printf("multimedia controller             "); break;
    case PCI_MEMORY_CONTROLLER:                  printf("memory controller                 "); break;
    case PCI_BRIDGE_DEVICE:                      printf("bridge device                     "); break;
    case PCI_SIMPLE_COMMUNICATION_CONTROLLER:    printf("simple communication controller   "); break;
    case PCI_BASE_SYSTEM_PERIPHERAL:             printf("base system peripheral            "); break;
    case PCI_INPUT_DEVICE_CONTROLLER:            printf("input device controller           "); break;
    case PCI_DOCKING_STATION:                    printf("docking station                   "); break;
    case PCI_PROCESSOR:                          printf("processor                         "); break;
    case PCI_SERIAL_BUS_CONTROLLER:              printf("serial bus controller             "); break;
    case PCI_WIRELESS_CONTROLLER:                printf("wireless controller               "); break;
    case PCI_INTELLIGENT_CONTROLLER:             printf("intelligent controller            "); break;
    case PCI_SATELLITE_COMMUNICATION_CONTROLLER: printf("satellite communication controller"); break;
    case PCI_ENCRYPTION_CONTROLLER:              printf("encryption_controller             "); break;
    case PCI_SIGNAL_PROCESSING_CONTROLLER:       printf("signal processing controller      "); break;
    case PCI_PROCESSING_ACCELERATOR:             printf("processing accelerator            "); break;
    case PCI_NONESSENTIAL_INSTRUMENTATION:       printf("nonessential instrumentation      "); break;
    case PCI_COPROCESSOR:                        printf("coprocessor                       "); break;
    case PCI_UNASSIGNED:                         printf("unassigned                        "); break;
    }
	printf("\n");
}

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

	set_color(COLOR_GREEN, COLOR_BLACK);
    printf("PCI Dump\n");
    set_color(COLOR_DARK_GREY, COLOR_BLACK);
    printf("vendor device class subclass description\n");
    set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
	enumerate_pci(kernel_pci_enumerator);
	printf("\n");

	// Enable interrupts once initialization is done.
    asm volatile("sti");

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

	for(;;);
}
