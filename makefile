OUTPUT_FILE := proto.iso

.PHONY: all clean

all: $(OUTPUT_FILE)

# DEFINES = -DNO_GFX

include kernel/makefile

$(OUTPUT_FILE): $(KERNEL_OUT) grub.cfg
	@mkdir -p iso/boot/grub
	@cp $(KERNEL_DIR)/$(KERNEL_OUT) iso/boot/kernel
	@cp grub.cfg iso/boot/grub/grub.cfg
	@grub-mkrescue -o $(OUTPUT_FILE) iso > /dev/null 2>&1
	@echo [..] done : $(OUTPUT_FILE)

clean: kernel_clean
	@rm -rf iso
	@rm -f $(OUTPUT_FILE)
