.intel_syntax noprefix

# void idt_flush(idt_ptr_t* ptr);
.global idt_flush
idt_flush:
	mov	eax, [esp + 4]
	lidt [eax]
    ret
