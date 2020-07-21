#ifndef _KMEM_H
#define _KMEM_H

#include <stdint.h>
#include <stddef.h>

// Early kernel allocator.

// Allocate 'size' bytes, if 'align' == 1, the returned address will be aligned
// on a page boundary. The physical address of the allocation is returned in 'phys'.
uint32_t kmem_alloc_int(size_t size, int align, uint32_t* phys);

// Allocates 'size' bytes, page aligned.
uint32_t kmem_alloc_a(size_t size);

// Allocates 'size' bytes, physical address is returned in 'phys'.
uint32_t kmem_alloc_p(size_t size, uint32_t* phys);

// Allocates 'size' bytes, page aligned, physical address is returned in 'phys'.
uint32_t kmem_alloc_ap(size_t size, uint32_t* phys);

// Allocates 'size' bytes.
uint32_t kmem_alloc(size_t size);

#endif
