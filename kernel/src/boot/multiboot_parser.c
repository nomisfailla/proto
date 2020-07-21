#include "multiboot_parser.h"

static void default_capabilities(multiboot_capabilities_t* caps)
{
	caps->framebuffer.type = FB_TEXT;
	caps->framebuffer.address = 0xB8000;
	caps->framebuffer.width = 80;
	caps->framebuffer.height = 25;
	caps->framebuffer.pitch = 80 * 2;
	caps->framebuffer.bpp = 16;
	caps->framebuffer.using_bga = false;
}

static void parse_framebuffer(multiboot_capabilities_t* caps, struct multiboot_tag_framebuffer* tag)
{
	if(caps->framebuffer.using_bga)
	{
		return;
	}

	caps->framebuffer.address = tag->common.framebuffer_addr;
	caps->framebuffer.width = tag->common.framebuffer_width;
	caps->framebuffer.height = tag->common.framebuffer_height;
	caps->framebuffer.pitch = tag->common.framebuffer_pitch;
	caps->framebuffer.bpp = tag->common.framebuffer_bpp;
	
	switch(tag->common.framebuffer_type)
	{
	case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED: {
		kdebug("Unsupported framebuffer mode (INDEXED).\n");
		for (;;);
	} break;
	case MULTIBOOT_FRAMEBUFFER_TYPE_RGB: {
		caps->framebuffer.type = FB_PIXELS;
		caps->framebuffer.format.red_position = tag->framebuffer_red_field_position;
		caps->framebuffer.format.red_mask_size = tag->framebuffer_red_mask_size;
		caps->framebuffer.format.green_position = tag->framebuffer_green_field_position;
		caps->framebuffer.format.green_mask_size = tag->framebuffer_green_mask_size;
		caps->framebuffer.format.blue_position = tag->framebuffer_blue_field_position;
		caps->framebuffer.format.blue_mask_size = tag->framebuffer_blue_mask_size;
	} break;
	case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT: {
		caps->framebuffer.type = FB_TEXT;
	} break;
	}
}

static void parse_cmdline(multiboot_capabilities_t* caps, struct multiboot_tag_string* tag)
{
	
}

multiboot_capabilities_t parse_multiboot(uint32_t start_addr, uint32_t magic)
{
	kdebug("Parsing Multiboot structure\n");

	if(magic != MULTIBOOT2_BOOTLOADER_MAGIC)
	{
		kdebug("Invalid Multiboot magic number: %x\n", magic);
	}

	if(start_addr & 7)
	{
		kdebug("Invalid Multiboot alignment\n");
	}

	multiboot_capabilities_t caps;
	default_capabilities(&caps);

	struct multiboot_tag *tag;
	for (tag = (struct multiboot_tag *) (start_addr + 8);
		 tag->type != MULTIBOOT_TAG_TYPE_END;
		 tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag  + ((tag->size + 7) & ~7)))
	{
		switch(tag->type)
		{
		case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME: kdebug("Unhandled tag BOOT_LOADER_NAME\n"); break;
		case MULTIBOOT_TAG_TYPE_MODULE:           kdebug("Unhandled tag MODULE\n");           break;
		case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:    kdebug("Unhandled tag BASIC_MEMINFO\n");    break;
		case MULTIBOOT_TAG_TYPE_BOOTDEV:          kdebug("Unhandled tag BOOTDEV\n");          break;
		case MULTIBOOT_TAG_TYPE_MMAP:             kdebug("Unhandled tag MMAP\n");             break;
		case MULTIBOOT_TAG_TYPE_VBE:              kdebug("Unhandled tag VBE\n");              break;
		case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:     kdebug("Unhandled tag ELF_SECTIONS\n");     break;
		case MULTIBOOT_TAG_TYPE_APM:              kdebug("Unhandled tag APM\n");              break;
		case MULTIBOOT_TAG_TYPE_EFI32:            kdebug("Unhandled tag EFI32\n");            break;
		case MULTIBOOT_TAG_TYPE_EFI64:            kdebug("Unhandled tag EFI64\n");            break;
		case MULTIBOOT_TAG_TYPE_SMBIOS:           kdebug("Unhandled tag SMBIOS\n");           break;
		case MULTIBOOT_TAG_TYPE_ACPI_OLD:         kdebug("Unhandled tag ACPI_OLD\n");         break;
		case MULTIBOOT_TAG_TYPE_ACPI_NEW:         kdebug("Unhandled tag ACPI_NEW\n");         break;
		case MULTIBOOT_TAG_TYPE_NETWORK:          kdebug("Unhandled tag NETWORK\n");          break;
		case MULTIBOOT_TAG_TYPE_EFI_MMAP:         kdebug("Unhandled tag EFI_MMAP\n");         break;
		case MULTIBOOT_TAG_TYPE_EFI_BS:           kdebug("Unhandled tag EFI_BS\n");           break;
		case MULTIBOOT_TAG_TYPE_EFI32_IH:         kdebug("Unhandled tag EFI32_IH\n");         break;
		case MULTIBOOT_TAG_TYPE_EFI64_IH:         kdebug("Unhandled tag EFI64_IH\n");         break;
		case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR:   kdebug("Unhandled tag LOAD_BASE_ADDR\n");   break;
		case MULTIBOOT_TAG_TYPE_CMDLINE:          parse_cmdline    (&caps, (struct multiboot_tag_string*     )tag);  break;
		case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:      parse_framebuffer(&caps, (struct multiboot_tag_framebuffer*)tag); break;
		}
	}

	return caps;
}
