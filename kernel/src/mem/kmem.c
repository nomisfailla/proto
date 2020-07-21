#include "kmem.h"

extern uint32_t kernel_end;
uint32_t kmem_placement_address = (uint32_t)&kernel_end;

uint32_t kmem_alloc_int(size_t size, int align, uint32_t* phys)
{
    if(align == 1 && (kmem_placement_address & 0x00000FFF))
    {
        kmem_placement_address &= 0xFFFFF000;
        kmem_placement_address += 0x1000;
    }

    if(phys)
    {
        *phys = kmem_placement_address;
    }
 
    uint32_t result = kmem_placement_address;
    kmem_placement_address += size;
    return result;
}

uint32_t kmem_alloc_a(size_t size)
{
    return kmem_alloc_int(size, 1, 0);
}

uint32_t kmem_alloc_p(size_t size, uint32_t* phys)
{
    return kmem_alloc_int(size, 0, phys);
}

uint32_t kmem_alloc_ap(size_t size, uint32_t* phys)
{
    return kmem_alloc_int(size, 1, phys);
}

uint32_t kmem_alloc(size_t size)
{
    return kmem_alloc_int(size, 0, 0);
}
