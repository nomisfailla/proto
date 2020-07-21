.global mboot
.section .multiboot_header
.align 4

mboot:
    .int  0xe85250d6                              # Magic
    .int  0                                       # Architecture
    .int  mboot_end-mboot                         # Header length
    .int  -(0xe85250d6 + 0 + (mboot_end - mboot)) # Checksum

    # Multiboot tags.
    # Read the multiboot spec for a list of valid tags.

    # Framebuffer tag. Desipite the gfx mode being set in grub.cfg, this tag must be here to 
    # indicate support for a graphics mode, otherwise GRUB will not set if for us.
    # Note that GRUB will use the values defined in grub.cfg rather than these values.
    # Uncommenting this will make the kernel boot into graphics mode.
#ifndef NO_GFX
    .hword 5             # Type
    .hword 0             # Flags
    .word 20             # Size of 20
    .word 1024           # Width
    .word 768            # Height
    .word 24             # BPP
#endif
    # Terminating tag.
    .hword 0             # Type
    .hword 0             # Flags
    .word  8             # Size of 8
mboot_end:

.section .bss
.align 16
stack_bottom:
.space 16384
stack_top:

.section .text
.global start
start:
    mov $stack_top, %esp # Stack pointer

    push %eax            # eax should contain the magic value '0x36d76289'
    push %ebx            # ebx contains the address of the multiboot info structure

.extern kmain
    call kmain
hang:
    cli
    hlt
    jmp hang
