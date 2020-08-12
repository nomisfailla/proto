#ifndef _KMEM_H
#define _KMEM_H

#include <stdint.h>
#include <stddef.h>

#include "../term/terminal.h"
#include "../boot/multiboot_parser.h"
#include "../debug/debug.h"

#define PAGE_SIZE 4096

void init_pmm(multiboot_capabilities_t* caps);

uint32_t pmm_alloc_frame();
void pmm_free_frame(uint32_t frame);

#endif
