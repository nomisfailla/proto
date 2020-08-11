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

	caps->memory.kernel_base = 0;
	caps->memory.kernel_end = 0;
	caps->memory.multiboot_base = 0;
	caps->memory.multiboot_end = 0;
	caps->memory.highest_address = 0;
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

static void parse_basic_meminfo(multiboot_capabilities_t* caps, struct multiboot_tag_basic_meminfo* tag)
{
	caps->memory.highest_address = tag->mem_upper * 1024;
}

static void parse_load_base_addr(multiboot_capabilities_t* caps, struct multiboot_tag_load_base_addr* tag)
{
	caps->memory.kernel_base = tag->load_base_addr;
}

static void parse_memmap(multiboot_capabilities_t* caps, struct multiboot_tag_mmap* tag)
{
	caps->memory.multiboot_memory_map = tag;
}

static char* get_from_string_table(struct multiboot_tag_elf_sections* tag, uint32_t idx)
{
	elf_section_header_t* header = &((elf_section_header_t*)tag->sections)[tag->shndx];
	return (char*)(header->sh_addr + idx);
}

static void parse_elf_sections(multiboot_capabilities_t* caps, struct multiboot_tag_elf_sections* tag)
{
	elf_section_header_t* cur_header = (elf_section_header_t*)tag->sections;

	uint32_t kernel_end = 0;
	kdebug("ELF sections\n");
	for(uint32_t i = 0; i < tag->num; i++)
	{
		kdebug(" - %s\n", get_from_string_table(tag, cur_header->sh_name));

		kernel_end = cur_header->sh_addr > kernel_end ? cur_header->sh_addr : kernel_end;
		cur_header++;
	}

	caps->memory.kernel_end = kernel_end;
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
	caps.memory.multiboot_base = start_addr;
	caps.memory.multiboot_end = start_addr + *((uint32_t*)start_addr);

	struct multiboot_tag *tag;
	for (tag = (struct multiboot_tag *) (start_addr + 8);
		 tag->type != MULTIBOOT_TAG_TYPE_END;
		 tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag  + ((tag->size + 7) & ~7)))
	{
		switch(tag->type)
		{
		case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME: kdebug("Unhandled tag BOOT_LOADER_NAME\n"); break;
		case MULTIBOOT_TAG_TYPE_MODULE:           kdebug("Unhandled tag MODULE\n");           break;
		case MULTIBOOT_TAG_TYPE_BOOTDEV:          kdebug("Unhandled tag BOOTDEV\n");          break;
		case MULTIBOOT_TAG_TYPE_VBE:              kdebug("Unhandled tag VBE\n");              break;
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
		case MULTIBOOT_TAG_TYPE_CMDLINE:          kdebug("Unhandled tag CMDLINE\n");          break;
		case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR:   parse_load_base_addr(&caps, (struct multiboot_tag_load_base_addr*) tag); break;
		case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:    parse_basic_meminfo (&caps, (struct multiboot_tag_basic_meminfo*)  tag); break;
		case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:     parse_elf_sections  (&caps, (struct multiboot_tag_elf_sections*)   tag); break;
		case MULTIBOOT_TAG_TYPE_MMAP:             parse_memmap        (&caps, (struct multiboot_tag_mmap*)           tag); break;
		case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:      parse_framebuffer   (&caps, (struct multiboot_tag_framebuffer*)    tag); break;
		}
	}

	return caps;
}
