#ifndef _IDT_H
#define _IDT_H

#include <stdint.h>

#include "../cpu/ports.h"

typedef union
{
    struct
    {
        uint16_t base_lo;
        uint16_t sel;
        uint8_t always0;
        uint8_t flags;
        uint16_t base_hi;
    };
    struct
    {
        uint32_t low;
        uint32_t high;
    };
} __attribute__((packed)) idt_entry_t;

typedef struct
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

typedef struct
{
    uint32_t ss;
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint16_t exception_code;
    uint16_t isr_number;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t userspace_esp;
    uint32_t userspace_ss;
} __attribute__((packed)) registers_t;

void init_idt();

#endif
