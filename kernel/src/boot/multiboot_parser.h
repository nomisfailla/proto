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
} multiboot_capabilities_t;

multiboot_capabilities_t parse_multiboot(uint32_t start_addr, uint32_t magic);

#endif
