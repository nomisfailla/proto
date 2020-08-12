#include "pmm.h"

static memory_info_t mem_info;
static uint32_t* mem_bitset;

#define SET_BIT(n) mem_bitset[(n) / 32] |= (1 << ((n) % 32))
#define CLEAR_BIT(n) mem_bitset[(n) / 32] &= ~(1 << ((n) % 32))
#define GET_BIT(n) mem_bitset[(n) / 32] & (1 << ((n) % 32))

static bool is_frame_reserved(uint32_t addr)
{
    const uint32_t addr_end = addr + PAGE_SIZE;

    // Ultra-low addresses are always reserved.
    if (addr_end <= 0x1000) return true;

    // Kernel location is reserved.
    if (
        (addr     >= mem_info.kernel_base && addr     <= mem_info.kernel_end) ||
        (addr_end >= mem_info.kernel_base && addr_end <= mem_info.kernel_end)
    ) {
        return true;
    }

    // Try not to destroy the multiboot info.
    if (
        (addr     >= mem_info.multiboot_base && addr     <= mem_info.multiboot_end) ||
        (addr_end >= mem_info.multiboot_base && addr_end <= mem_info.multiboot_end)
    ) {
        return true;
    }

    // Inspect the multiboot memory map.
    const uint32_t entry_size = mem_info.multiboot_memory_map->entry_size;
    const uint32_t table_size = mem_info.multiboot_memory_map->size;
    for (multiboot_memory_map_t* mmap = mem_info.multiboot_memory_map->entries;
		(multiboot_uint8_t*) mmap < (multiboot_uint8_t*)mem_info.multiboot_memory_map + table_size;
        mmap = (multiboot_memory_map_t*)((uint32_t) mmap + entry_size))
	{
        const uint32_t start = mmap->addr;
        const uint32_t end = start + mmap->len;

        if (addr >= start && addr < end)
        {
            if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE) return true;
            if (addr + PAGE_SIZE > end) return true;

            return false;
        }
	}

    // If we reached this point, the memory most likely doesn't exist.
    return true;
}

void init_pmm(multiboot_capabilities_t* caps)
{
    mem_info = caps->memory;

	kdebug("Highest address is: 0x%x\n", mem_info.highest_address);
	kdebug("Kernel is at 0x%x - 0x%x (%d bytes)\n",
		mem_info.kernel_base,
		mem_info.kernel_end,
		mem_info.kernel_end - mem_info.kernel_base
	);
	kdebug("Multiboot is at: 0x%x - 0x%x (%d bytes)\n",
		mem_info.multiboot_base,
		mem_info.multiboot_end,
		mem_info.multiboot_end - mem_info.multiboot_base
	);
    kdebug("Framebuffer is at: 0x%x - 0x%x (%d bytes)\n",
		caps->framebuffer.address,
		caps->framebuffer.address + (caps->framebuffer.pitch * caps->framebuffer.height),
		(caps->framebuffer.pitch * caps->framebuffer.height)
	);

    const uint32_t bitset_length = mem_info.highest_address / 0x1000;
    const uint32_t bitset_size = (((bitset_length - 1) / 32) + 1) * sizeof(uint32_t);
    kdebug("Bitset length: %d\n", bitset_length);
    kdebug("Bitset size: %d bytes\n", bitset_size);

    for (uint32_t i = 0; i + PAGE_SIZE < mem_info.highest_address; i += PAGE_SIZE)
    {
        if (is_frame_reserved(i))
        {
            SET_BIT(i / PAGE_SIZE);
        }
        else
        {
            CLEAR_BIT(i / PAGE_SIZE);
        }
    }
}

static uint32_t next_free_frame()
{
    for(uint32_t i = 0; i < mem_info.highest_address / PAGE_SIZE / 32; i++)
    {
        if(mem_bitset[i] == 0xFFFFFFFF) continue;

        for(uint32_t j = 0; j < 32; j++)
        {
            if(!(mem_bitset[i] & (1 << j)))
            {
                return i * 32 + j;
            }
        }
    }

    kdebug("!!! Failed to get free frame, out of memory !!!");
    return 0;
}

uint32_t pmm_alloc_frame()
{
    uint32_t next = next_free_frame();
    
    if(next != 0) SET_BIT(next);
    return next;
}

void pmm_free_frame(uint32_t frame)
{
    CLEAR_BIT(frame);
}
