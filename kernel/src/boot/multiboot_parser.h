#ifndef _MULTIBOOT_PARSER_H
#define _MULTIBOOT_PARSER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "multiboot2.h"
#include "../debug/debug.h"

typedef enum
{
	FB_TEXT,
	FB_PIXELS
} framebuffer_type_t;

typedef struct
{
    uint32_t sh_name;
    uint32_t sh_type;
    uint32_t sh_flags;
    uint32_t sh_addr;
    uint32_t sh_offset;
    uint32_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint32_t sh_addralign;
    uint32_t sh_entsize;
} elf_section_header_t;

typedef struct
{
	struct
	{
		framebuffer_type_t type;
		uint32_t address;
		size_t width;
		size_t height;
		size_t pitch;
		size_t bpp;
		struct
		{
			uint8_t red_position;
			uint8_t red_mask_size;
			uint8_t green_position;
			uint8_t green_mask_size;
			uint8_t blue_position;
			uint8_t blue_mask_size;
		} format;
		bool using_bga;
	} framebuffer;

	struct
	{
		uint32_t kernel_base;
		uint32_t kernel_end;

		uint32_t multiboot_base;
		uint32_t multiboot_end;

		uint32_t highest_address;

		multiboot_tag_mmap_t* multiboot_memory_map;
	} memory;
} multiboot_capabilities_t;

multiboot_capabilities_t parse_multiboot(uint32_t start_addr, uint32_t magic);

#endif
