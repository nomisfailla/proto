#include "gdt.h"

#define GDT_SIZE 5

static gdt_entry_t gdt[GDT_SIZE];
static gdt_ptr_t gdt_ptr;

extern void gdt_flush(gdt_ptr_t* ptr);

static void gdt_set(int32_t num, uint64_t base, uint64_t limit, uint8_t access, uint8_t gran)
{
	gdt[num].base_low = (base & 0xFFFF);
	gdt[num].base_middle = (base >> 16) & 0xFF;
	gdt[num].base_high = (base >> 24) & 0xFF;
	gdt[num].limit_low = (limit & 0xFFFF);
	gdt[num].granularity = ((limit >> 16) & 0x0F);
	gdt[num].granularity |= (gran & 0xF0);
	gdt[num].access = access;
}

void init_gdt()
{
	gdt_ptr.limit = (sizeof(gdt_entry_t) * GDT_SIZE) - 1;
	gdt_ptr.base = (uint32_t)&gdt;

	gdt_set(0, 0, 0, 0, 0);
	gdt_set(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
	gdt_set(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
	gdt_set(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
	gdt_set(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

	//TSS
	//uint32_t tss = (uint32_t)tss_return();
	//gdt_set(3, tss, sizeof(tss_t), 0x89, 0xCF);

	gdt_flush(&gdt_ptr);
}
