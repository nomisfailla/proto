#include "idt.h"

#include "../term/terminal.h"
#include "../debug/debug.h"

#define IDT_SIZE 256

static idt_entry_t idt[IDT_SIZE];
static idt_ptr_t idt_ptr;

static void set_entry(uint8_t idx, void(*func)())
{
    idt[idx].low = 0x80000 | ((uint32_t)func & 0xFFFF);
    idt[idx].high = ((uint32_t)func & 0xffff0000) | 0x8e00;
}

void unhandled_interrupt()
{
    // TODO: hang
}

#define EXCEPTION_ENTRY_CODE(ec, title)             \
    extern void _exception##ec();                   \
    extern void title##_handler(registers_t);       \
    asm(                                            \
        ".globl _exception" #ec "\n"                \
        "_exception" #ec ":\n"                      \
        "    pusha\n"                               \
        "    pushl %ds\n"                           \
        "    pushl %es\n"                           \
        "    pushl %fs\n"                           \
        "    pushl %gs\n"                           \
        "    pushl %ss\n"                           \
        "    mov $0x10, %ax\n"                      \
        "    mov %ax, %ds\n"                        \
        "    mov %ax, %es\n"                        \
        "    cld\n"                                 \
        "    call " #title "_handler\n"             \
        "    add $0x4, %esp \n"                     \
        "    popl %gs\n"                            \
        "    popl %fs\n"                            \
        "    popl %es\n"                            \
        "    popl %ds\n"                            \
        "    popa\n"                                \
        "    add $0x4, %esp\n"                      \
        "    iret\n");

#define EXCEPTION_ENTRY(ec, title)                  \
    extern void _exception##ec();                   \
    extern void title##_handler(registers_t);       \
    asm(                                            \
        ".globl _exception" #ec "\n"                \
        "_exception" #ec ":\n"                      \
        "    pushl $0x0\n"                          \
        "    pusha\n"                               \
        "    pushl %ds\n"                           \
        "    pushl %es\n"                           \
        "    pushl %fs\n"                           \
        "    pushl %gs\n"                           \
        "    pushl %ss\n"                           \
        "    mov $0x10, %ax\n"                      \
        "    mov %ax, %ds\n"                        \
        "    mov %ax, %es\n"                        \
        "    cld\n"                                 \
        "    call " #title "_handler\n"             \
        "    add $0x4, %esp\n"                      \
        "    popl %gs\n"                            \
        "    popl %fs\n"                            \
        "    popl %es\n"                            \
        "    popl %ds\n"                            \
        "    popa\n"                                \
        "    add $0x4, %esp\n"                      \
        "    iret\n");

#define ERROR_STUB(code, desc)            \
void desc##_handler(registers_t regs)     \
{                                         \
    ((void)regs);                         \
    puts(#desc);                          \
    kdebug("exception: " #desc "\n");     \
    for(;;);                              \
}                                         \

EXCEPTION_ENTRY(0, divide_by_zero)
ERROR_STUB(0, divide_by_zero)

EXCEPTION_ENTRY(1, debug)
ERROR_STUB(1, debug)

EXCEPTION_ENTRY(2, non_maskable_interrupt)
ERROR_STUB(2, non_maskable_interrupt)

EXCEPTION_ENTRY(3, breakpoint)
ERROR_STUB(3, breakpoint)

EXCEPTION_ENTRY(4, overflow)
ERROR_STUB(4, overflow)

EXCEPTION_ENTRY(5, bound_range_exceeded)
ERROR_STUB(5, bound_range_exceeded)

EXCEPTION_ENTRY(6, invalid_opcode)
ERROR_STUB(6, invalid_opcode)

EXCEPTION_ENTRY(7, device_unavailable)
ERROR_STUB(7, device_unavailable)

EXCEPTION_ENTRY_CODE(8, double_fault)
ERROR_STUB(8, double_fault)

EXCEPTION_ENTRY(9, coprocessor_segment_overrun)
ERROR_STUB(9, coprocessor_segment_overrun)

EXCEPTION_ENTRY_CODE(10, invalid_tss)
ERROR_STUB(10, invalid_tss)

EXCEPTION_ENTRY_CODE(11, segment_not_present)
ERROR_STUB(11, segment_not_present)

EXCEPTION_ENTRY_CODE(12, stack_segment_fault)
ERROR_STUB(12, stack_segment_fault)

EXCEPTION_ENTRY_CODE(13, general_protection_fault)
ERROR_STUB(13, general_protection_fault)

EXCEPTION_ENTRY_CODE(14, page_fault)
ERROR_STUB(13, page_fault)

EXCEPTION_ENTRY(15, reserved_15)
ERROR_STUB(15, reserved_15)

EXCEPTION_ENTRY(16, floating_point_fault)
ERROR_STUB(16, floating_point_fault)

EXCEPTION_ENTRY_CODE(17, alignment_check)
ERROR_STUB(17, alignment_check)

EXCEPTION_ENTRY(18, machine_check)
ERROR_STUB(18, machine_check)

EXCEPTION_ENTRY(19, simd_fault)
ERROR_STUB(19, simd_fault)

EXCEPTION_ENTRY(20, virtualization_exception)
ERROR_STUB(20, virtualization_exception)

EXCEPTION_ENTRY(21, reserved_21)
ERROR_STUB(21, reserved_21)

EXCEPTION_ENTRY(22, reserved_22)
ERROR_STUB(22, reserved_22)

EXCEPTION_ENTRY(23, reserved_23)
ERROR_STUB(23, reserved_23)

EXCEPTION_ENTRY(24, reserved_24)
ERROR_STUB(24, reserved_24)

EXCEPTION_ENTRY(25, reserved_25)
ERROR_STUB(25, reserved_25)

EXCEPTION_ENTRY(26, reserved_26)
ERROR_STUB(26, reserved_26)

EXCEPTION_ENTRY(27, reserved_27)
ERROR_STUB(27, reserved_27)

EXCEPTION_ENTRY(28, reserved_28)
ERROR_STUB(28, reserved_28)

EXCEPTION_ENTRY(29, reserved_29)
ERROR_STUB(29, reserved_29)

EXCEPTION_ENTRY(30, security_exception)
ERROR_STUB(30, security_exception)

EXCEPTION_ENTRY(31, reserved_31)
ERROR_STUB(31, reserved_31)

extern void interrupt_common_asm_entry();

#define INTERRUPT_ENTRY(interrupt_vector, isr_number) \
    extern void _interrupt ##interrupt_vector();                                   \
    asm(".globl _interrupt" #interrupt_vector "\n"                                 \
        "_interrupt"#interrupt_vector ":\n"                                        \
        "    pushw $" #isr_number "\n"                                             \
        "    pushw $0\n"                                                           \
        "    jmp interrupt_common_asm_entry\n");

asm(
    ".globl interrupt_common_asm_entry\n"
    "interrupt_common_asm_entry: \n"
    "    pusha\n"
    "    pushl %ds\n"
    "    pushl %es\n"
    "    pushl %fs\n"
    "    pushl %gs\n"
    "    pushl %ss\n"
    "    mov $0x10, %ax\n"
    "    mov %ax, %ds\n"
    "    mov %ax, %es\n"
    "    cld\n"
    "    call handle_interrupt\n"
    "    add $0x4, %esp\n"
    "    popl %gs\n"
    "    popl %fs\n"
    "    popl %es\n"
    "    popl %ds\n"
    "    popa\n"
    "    add $0x4, %esp\n"
    "    iret\n");

INTERRUPT_ENTRY(0, 0x20)
INTERRUPT_ENTRY(1, 0x21)
INTERRUPT_ENTRY(2, 0x22)
INTERRUPT_ENTRY(3, 0x23)
INTERRUPT_ENTRY(4, 0x24)
INTERRUPT_ENTRY(5, 0x25)
INTERRUPT_ENTRY(6, 0x26)
INTERRUPT_ENTRY(7, 0x27)
INTERRUPT_ENTRY(8, 0x28)
INTERRUPT_ENTRY(9, 0x29)
INTERRUPT_ENTRY(10, 0x2a)
INTERRUPT_ENTRY(11, 0x2b)
INTERRUPT_ENTRY(12, 0x2c)
INTERRUPT_ENTRY(13, 0x2d)
INTERRUPT_ENTRY(14, 0x2e)
INTERRUPT_ENTRY(15, 0x2f)
INTERRUPT_ENTRY(16, 0x30)
INTERRUPT_ENTRY(17, 0x31)
INTERRUPT_ENTRY(18, 0x32)
INTERRUPT_ENTRY(19, 0x33)
INTERRUPT_ENTRY(20, 0x34)
INTERRUPT_ENTRY(21, 0x35)
INTERRUPT_ENTRY(22, 0x36)
INTERRUPT_ENTRY(23, 0x37)
INTERRUPT_ENTRY(24, 0x38)
INTERRUPT_ENTRY(25, 0x39)
INTERRUPT_ENTRY(26, 0x3a)
INTERRUPT_ENTRY(27, 0x3b)
INTERRUPT_ENTRY(28, 0x3c)
INTERRUPT_ENTRY(29, 0x3d)
INTERRUPT_ENTRY(30, 0x3e)
INTERRUPT_ENTRY(31, 0x3f)
INTERRUPT_ENTRY(32, 0x40)
INTERRUPT_ENTRY(33, 0x41)
INTERRUPT_ENTRY(34, 0x42)
INTERRUPT_ENTRY(35, 0x43)
INTERRUPT_ENTRY(36, 0x44)
INTERRUPT_ENTRY(37, 0x45)
INTERRUPT_ENTRY(38, 0x46)
INTERRUPT_ENTRY(39, 0x47)
INTERRUPT_ENTRY(40, 0x48)
INTERRUPT_ENTRY(41, 0x49)
INTERRUPT_ENTRY(42, 0x4a)
INTERRUPT_ENTRY(43, 0x4b)
INTERRUPT_ENTRY(44, 0x4c)
INTERRUPT_ENTRY(45, 0x4d)
INTERRUPT_ENTRY(46, 0x4e)
INTERRUPT_ENTRY(47, 0x4f)
INTERRUPT_ENTRY(48, 0x50)
INTERRUPT_ENTRY(49, 0x51)
INTERRUPT_ENTRY(50, 0x52)
INTERRUPT_ENTRY(51, 0x53)
INTERRUPT_ENTRY(52, 0x54)
INTERRUPT_ENTRY(53, 0x55)
INTERRUPT_ENTRY(54, 0x56)
INTERRUPT_ENTRY(55, 0x57)
INTERRUPT_ENTRY(56, 0x58)
INTERRUPT_ENTRY(57, 0x59)
INTERRUPT_ENTRY(58, 0x5a)
INTERRUPT_ENTRY(59, 0x5b)
INTERRUPT_ENTRY(60, 0x5c)
INTERRUPT_ENTRY(61, 0x5d)
INTERRUPT_ENTRY(62, 0x5e)
INTERRUPT_ENTRY(63, 0x5f)
INTERRUPT_ENTRY(64, 0x60)
INTERRUPT_ENTRY(65, 0x61)
INTERRUPT_ENTRY(66, 0x62)
INTERRUPT_ENTRY(67, 0x63)
INTERRUPT_ENTRY(68, 0x64)
INTERRUPT_ENTRY(69, 0x65)
INTERRUPT_ENTRY(70, 0x66)
INTERRUPT_ENTRY(71, 0x67)
INTERRUPT_ENTRY(72, 0x68)
INTERRUPT_ENTRY(73, 0x69)
INTERRUPT_ENTRY(74, 0x6a)
INTERRUPT_ENTRY(75, 0x6b)
INTERRUPT_ENTRY(76, 0x6c)
INTERRUPT_ENTRY(77, 0x6d)
INTERRUPT_ENTRY(78, 0x6e)
INTERRUPT_ENTRY(79, 0x6f)
INTERRUPT_ENTRY(80, 0x70)
INTERRUPT_ENTRY(81, 0x71)
INTERRUPT_ENTRY(82, 0x72)
INTERRUPT_ENTRY(83, 0x73)
INTERRUPT_ENTRY(84, 0x74)
INTERRUPT_ENTRY(85, 0x75)
INTERRUPT_ENTRY(86, 0x76)
INTERRUPT_ENTRY(87, 0x77)
INTERRUPT_ENTRY(88, 0x78)
INTERRUPT_ENTRY(89, 0x79)
INTERRUPT_ENTRY(90, 0x7a)
INTERRUPT_ENTRY(91, 0x7b)
INTERRUPT_ENTRY(92, 0x7c)
INTERRUPT_ENTRY(93, 0x7d)
INTERRUPT_ENTRY(94, 0x7e)
INTERRUPT_ENTRY(95, 0x7f)
INTERRUPT_ENTRY(96, 0x80)
INTERRUPT_ENTRY(97, 0x81)
INTERRUPT_ENTRY(98, 0x82)
INTERRUPT_ENTRY(99, 0x83)
INTERRUPT_ENTRY(100, 0x84)
INTERRUPT_ENTRY(101, 0x85)
INTERRUPT_ENTRY(102, 0x86)
INTERRUPT_ENTRY(103, 0x87)
INTERRUPT_ENTRY(104, 0x88)
INTERRUPT_ENTRY(105, 0x89)
INTERRUPT_ENTRY(106, 0x8a)
INTERRUPT_ENTRY(107, 0x8b)
INTERRUPT_ENTRY(108, 0x8c)
INTERRUPT_ENTRY(109, 0x8d)
INTERRUPT_ENTRY(110, 0x8e)
INTERRUPT_ENTRY(111, 0x8f)
INTERRUPT_ENTRY(112, 0x90)
INTERRUPT_ENTRY(113, 0x91)
INTERRUPT_ENTRY(114, 0x92)
INTERRUPT_ENTRY(115, 0x93)
INTERRUPT_ENTRY(116, 0x94)
INTERRUPT_ENTRY(117, 0x95)
INTERRUPT_ENTRY(118, 0x96)
INTERRUPT_ENTRY(119, 0x97)
INTERRUPT_ENTRY(120, 0x98)
INTERRUPT_ENTRY(121, 0x99)
INTERRUPT_ENTRY(122, 0x9a)
INTERRUPT_ENTRY(123, 0x9b)
INTERRUPT_ENTRY(124, 0x9c)
INTERRUPT_ENTRY(125, 0x9d)
INTERRUPT_ENTRY(126, 0x9e)
INTERRUPT_ENTRY(127, 0x9f)
INTERRUPT_ENTRY(128, 0xa0)
INTERRUPT_ENTRY(129, 0xa1)
INTERRUPT_ENTRY(130, 0xa2)
INTERRUPT_ENTRY(131, 0xa3)
INTERRUPT_ENTRY(132, 0xa4)
INTERRUPT_ENTRY(133, 0xa5)
INTERRUPT_ENTRY(134, 0xa6)
INTERRUPT_ENTRY(135, 0xa7)
INTERRUPT_ENTRY(136, 0xa8)
INTERRUPT_ENTRY(137, 0xa9)
INTERRUPT_ENTRY(138, 0xaa)
INTERRUPT_ENTRY(139, 0xab)
INTERRUPT_ENTRY(140, 0xac)
INTERRUPT_ENTRY(141, 0xad)
INTERRUPT_ENTRY(142, 0xae)
INTERRUPT_ENTRY(143, 0xaf)
INTERRUPT_ENTRY(144, 0xb0)
INTERRUPT_ENTRY(145, 0xb1)
INTERRUPT_ENTRY(146, 0xb2)
INTERRUPT_ENTRY(147, 0xb3)
INTERRUPT_ENTRY(148, 0xb4)
INTERRUPT_ENTRY(149, 0xb5)
INTERRUPT_ENTRY(150, 0xb6)
INTERRUPT_ENTRY(151, 0xb7)
INTERRUPT_ENTRY(152, 0xb8)
INTERRUPT_ENTRY(153, 0xb9)
INTERRUPT_ENTRY(154, 0xba)
INTERRUPT_ENTRY(155, 0xbb)
INTERRUPT_ENTRY(156, 0xbc)
INTERRUPT_ENTRY(157, 0xbd)
INTERRUPT_ENTRY(158, 0xbe)
INTERRUPT_ENTRY(159, 0xbf)
INTERRUPT_ENTRY(160, 0xc0)
INTERRUPT_ENTRY(161, 0xc1)
INTERRUPT_ENTRY(162, 0xc2)
INTERRUPT_ENTRY(163, 0xc3)
INTERRUPT_ENTRY(164, 0xc4)
INTERRUPT_ENTRY(165, 0xc5)
INTERRUPT_ENTRY(166, 0xc6)
INTERRUPT_ENTRY(167, 0xc7)
INTERRUPT_ENTRY(168, 0xc8)
INTERRUPT_ENTRY(169, 0xc9)
INTERRUPT_ENTRY(170, 0xca)
INTERRUPT_ENTRY(171, 0xcb)
INTERRUPT_ENTRY(172, 0xcc)
INTERRUPT_ENTRY(173, 0xcd)
INTERRUPT_ENTRY(174, 0xce)
INTERRUPT_ENTRY(175, 0xcf)
INTERRUPT_ENTRY(176, 0xd0)
INTERRUPT_ENTRY(177, 0xd1)
INTERRUPT_ENTRY(178, 0xd2)
INTERRUPT_ENTRY(179, 0xd3)
INTERRUPT_ENTRY(180, 0xd4)
INTERRUPT_ENTRY(181, 0xd5)
INTERRUPT_ENTRY(182, 0xd6)
INTERRUPT_ENTRY(183, 0xd7)
INTERRUPT_ENTRY(184, 0xd8)
INTERRUPT_ENTRY(185, 0xd9)
INTERRUPT_ENTRY(186, 0xda)
INTERRUPT_ENTRY(187, 0xdb)
INTERRUPT_ENTRY(188, 0xdc)
INTERRUPT_ENTRY(189, 0xdd)
INTERRUPT_ENTRY(190, 0xde)
INTERRUPT_ENTRY(191, 0xdf)
INTERRUPT_ENTRY(192, 0xe0)
INTERRUPT_ENTRY(193, 0xe1)
INTERRUPT_ENTRY(194, 0xe2)
INTERRUPT_ENTRY(195, 0xe3)
INTERRUPT_ENTRY(196, 0xe4)
INTERRUPT_ENTRY(197, 0xe5)
INTERRUPT_ENTRY(198, 0xe6)
INTERRUPT_ENTRY(199, 0xe7)
INTERRUPT_ENTRY(200, 0xe8)
INTERRUPT_ENTRY(201, 0xe9)
INTERRUPT_ENTRY(202, 0xea)
INTERRUPT_ENTRY(203, 0xeb)
INTERRUPT_ENTRY(204, 0xec)
INTERRUPT_ENTRY(205, 0xed)
INTERRUPT_ENTRY(206, 0xee)
INTERRUPT_ENTRY(207, 0xef)
INTERRUPT_ENTRY(208, 0xf0)
INTERRUPT_ENTRY(209, 0xf1)
INTERRUPT_ENTRY(210, 0xf2)
INTERRUPT_ENTRY(211, 0xf3)
INTERRUPT_ENTRY(212, 0xf4)
INTERRUPT_ENTRY(213, 0xf5)
INTERRUPT_ENTRY(214, 0xf6)
INTERRUPT_ENTRY(215, 0xf7)
INTERRUPT_ENTRY(216, 0xf8)
INTERRUPT_ENTRY(217, 0xf9)
INTERRUPT_ENTRY(218, 0xfa)
INTERRUPT_ENTRY(219, 0xfb)
INTERRUPT_ENTRY(220, 0xfc)
INTERRUPT_ENTRY(221, 0xfd)
INTERRUPT_ENTRY(222, 0xfe)
INTERRUPT_ENTRY(223, 0xff)
INTERRUPT_ENTRY(224, 0x100)
INTERRUPT_ENTRY(225, 0x101)
INTERRUPT_ENTRY(226, 0x102)
INTERRUPT_ENTRY(227, 0x103)
INTERRUPT_ENTRY(228, 0x104)
INTERRUPT_ENTRY(229, 0x105)
INTERRUPT_ENTRY(230, 0x106)
INTERRUPT_ENTRY(231, 0x107)
INTERRUPT_ENTRY(232, 0x108)
INTERRUPT_ENTRY(233, 0x109)
INTERRUPT_ENTRY(234, 0x10a)
INTERRUPT_ENTRY(235, 0x10b)
INTERRUPT_ENTRY(236, 0x10c)
INTERRUPT_ENTRY(237, 0x10d)
INTERRUPT_ENTRY(238, 0x10e)
INTERRUPT_ENTRY(239, 0x10f)
INTERRUPT_ENTRY(240, 0x110)
INTERRUPT_ENTRY(241, 0x111)
INTERRUPT_ENTRY(242, 0x112)
INTERRUPT_ENTRY(243, 0x113)
INTERRUPT_ENTRY(244, 0x114)
INTERRUPT_ENTRY(245, 0x115)
INTERRUPT_ENTRY(246, 0x116)
INTERRUPT_ENTRY(247, 0x117)
INTERRUPT_ENTRY(248, 0x118)
INTERRUPT_ENTRY(249, 0x119)
INTERRUPT_ENTRY(250, 0x11a)
INTERRUPT_ENTRY(251, 0x11b)
INTERRUPT_ENTRY(252, 0x11c)
INTERRUPT_ENTRY(253, 0x11d)
INTERRUPT_ENTRY(254, 0x11e)
INTERRUPT_ENTRY(255, 0x11f)

extern void idt_flush(idt_ptr_t* ptr);

void init_idt()
{
    idt_ptr.limit = (sizeof(idt_entry_t) * IDT_SIZE) - 1;
    idt_ptr.base = (uint32_t)&idt;

    idt_flush(&idt_ptr);

    for(int i = 0; i < IDT_SIZE; i++)
    {
        set_entry(i, unhandled_interrupt);
    }

    set_entry(0, _exception0);
    set_entry(1, _exception1);
    set_entry(2, _exception2);
    set_entry(3, _exception3);
    set_entry(4, _exception4);
    set_entry(5, _exception5);
    set_entry(6, _exception6);
    set_entry(7, _exception7);
    set_entry(8, _exception8);
    set_entry(9, _exception9);
    set_entry(10, _exception10);
    set_entry(11, _exception11);
    set_entry(12, _exception12);
    set_entry(13, _exception13);
    set_entry(14, _exception14);
    set_entry(15, _exception15);
    set_entry(16, _exception16);
    set_entry(17, _exception17);
    set_entry(18, _exception18);
    set_entry(19, _exception19);
    set_entry(20, _exception20);
    set_entry(21, _exception21);
    set_entry(22, _exception22);
    set_entry(23, _exception23);
    set_entry(24, _exception24);
    set_entry(25, _exception25);
    set_entry(26, _exception26);
    set_entry(27, _exception27);
    set_entry(28, _exception28);
    set_entry(29, _exception29);
    set_entry(30, _exception30);
    set_entry(31, _exception31);

    // Remap the PIC.
    out8(0x20, 0x11);
    out8(0xA0, 0x11);
    out8(0x21, 0x20);
    out8(0xA1, 0x28);
    out8(0x21, 0x04);
    out8(0xA1, 0x02);
    out8(0x21, 0x01);
    out8(0xA1, 0x01);
    out8(0x21, 0x0);
    out8(0xA1, 0x0);

    set_entry(0x20, _interrupt0);
    set_entry(0x21, _interrupt1);
    set_entry(0x22, _interrupt2);
    set_entry(0x23, _interrupt3);
    set_entry(0x24, _interrupt4);
    set_entry(0x25, _interrupt5);
    set_entry(0x26, _interrupt6);
    set_entry(0x27, _interrupt7);
    set_entry(0x28, _interrupt8);
    set_entry(0x29, _interrupt9);
    set_entry(0x2a, _interrupt10);
    set_entry(0x2b, _interrupt11);
    set_entry(0x2c, _interrupt12);
    set_entry(0x2d, _interrupt13);
    set_entry(0x2e, _interrupt14);
    set_entry(0x2f, _interrupt15);
    set_entry(0x30, _interrupt16);
    set_entry(0x31, _interrupt17);
    set_entry(0x32, _interrupt18);
    set_entry(0x33, _interrupt19);
    set_entry(0x34, _interrupt20);
    set_entry(0x35, _interrupt21);
    set_entry(0x36, _interrupt22);
    set_entry(0x37, _interrupt23);
    set_entry(0x38, _interrupt24);
    set_entry(0x39, _interrupt25);
    set_entry(0x3a, _interrupt26);
    set_entry(0x3b, _interrupt27);
    set_entry(0x3c, _interrupt28);
    set_entry(0x3d, _interrupt29);
    set_entry(0x3e, _interrupt30);
    set_entry(0x3f, _interrupt31);
    set_entry(0x40, _interrupt32);
    set_entry(0x41, _interrupt33);
    set_entry(0x42, _interrupt34);
    set_entry(0x43, _interrupt35);
    set_entry(0x44, _interrupt36);
    set_entry(0x45, _interrupt37);
    set_entry(0x46, _interrupt38);
    set_entry(0x47, _interrupt39);
    set_entry(0x48, _interrupt40);
    set_entry(0x49, _interrupt41);
    set_entry(0x4a, _interrupt42);
    set_entry(0x4b, _interrupt43);
    set_entry(0x4c, _interrupt44);
    set_entry(0x4d, _interrupt45);
    set_entry(0x4e, _interrupt46);
    set_entry(0x4f, _interrupt47);
    set_entry(0x50, _interrupt48);
    set_entry(0x51, _interrupt49);
    set_entry(0x52, _interrupt50);
    set_entry(0x53, _interrupt51);
    set_entry(0x54, _interrupt52);
    set_entry(0x55, _interrupt53);
    set_entry(0x56, _interrupt54);
    set_entry(0x57, _interrupt55);
    set_entry(0x58, _interrupt56);
    set_entry(0x59, _interrupt57);
    set_entry(0x5a, _interrupt58);
    set_entry(0x5b, _interrupt59);
    set_entry(0x5c, _interrupt60);
    set_entry(0x5d, _interrupt61);
    set_entry(0x5e, _interrupt62);
    set_entry(0x5f, _interrupt63);
    set_entry(0x60, _interrupt64);
    set_entry(0x61, _interrupt65);
    set_entry(0x62, _interrupt66);
    set_entry(0x63, _interrupt67);
    set_entry(0x64, _interrupt68);
    set_entry(0x65, _interrupt69);
    set_entry(0x66, _interrupt70);
    set_entry(0x67, _interrupt71);
    set_entry(0x68, _interrupt72);
    set_entry(0x69, _interrupt73);
    set_entry(0x6a, _interrupt74);
    set_entry(0x6b, _interrupt75);
    set_entry(0x6c, _interrupt76);
    set_entry(0x6d, _interrupt77);
    set_entry(0x6e, _interrupt78);
    set_entry(0x6f, _interrupt79);
    set_entry(0x70, _interrupt80);
    set_entry(0x71, _interrupt81);
    set_entry(0x72, _interrupt82);
    set_entry(0x73, _interrupt83);
    set_entry(0x74, _interrupt84);
    set_entry(0x75, _interrupt85);
    set_entry(0x76, _interrupt86);
    set_entry(0x77, _interrupt87);
    set_entry(0x78, _interrupt88);
    set_entry(0x79, _interrupt89);
    set_entry(0x7a, _interrupt90);
    set_entry(0x7b, _interrupt91);
    set_entry(0x7c, _interrupt92);
    set_entry(0x7d, _interrupt93);
    set_entry(0x7e, _interrupt94);
    set_entry(0x7f, _interrupt95);
    set_entry(0x80, _interrupt96);
    set_entry(0x81, _interrupt97);
    set_entry(0x82, _interrupt98);
    set_entry(0x83, _interrupt99);
    set_entry(0x84, _interrupt100);
    set_entry(0x85, _interrupt101);
    set_entry(0x86, _interrupt102);
    set_entry(0x87, _interrupt103);
    set_entry(0x88, _interrupt104);
    set_entry(0x89, _interrupt105);
    set_entry(0x8a, _interrupt106);
    set_entry(0x8b, _interrupt107);
    set_entry(0x8c, _interrupt108);
    set_entry(0x8d, _interrupt109);
    set_entry(0x8e, _interrupt110);
    set_entry(0x8f, _interrupt111);
    set_entry(0x90, _interrupt112);
    set_entry(0x91, _interrupt113);
    set_entry(0x92, _interrupt114);
    set_entry(0x93, _interrupt115);
    set_entry(0x94, _interrupt116);
    set_entry(0x95, _interrupt117);
    set_entry(0x96, _interrupt118);
    set_entry(0x97, _interrupt119);
    set_entry(0x98, _interrupt120);
    set_entry(0x99, _interrupt121);
    set_entry(0x9a, _interrupt122);
    set_entry(0x9b, _interrupt123);
    set_entry(0x9c, _interrupt124);
    set_entry(0x9d, _interrupt125);
    set_entry(0x9e, _interrupt126);
    set_entry(0x9f, _interrupt127);
}

void handle_interrupt(registers_t regs)
{
    uint32_t irq = regs.isr_number - 32;
    (void)irq;

    // handle interrupt now.
    //vga_textmode_printf("interrupt: %d\n", irq);

    // Acknowledge the interrupt.
    if(regs.isr_number >= 40)
    {
        // If the interrupt came from the slave PIC.
        out8(0xA0, 0x20);
    }
    out8(0x20, 0x20);
}
