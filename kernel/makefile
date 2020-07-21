KERNEL_DIR := kernel
KERNEL_OUT := proto.knl

CC = ~/opt/i686-elf-cross/bin/i686-elf-gcc
CFLAGS = -ffreestanding -O3 -Wall -Werror -Wpedantic
LDFLAGS = -nostdlib -lgcc -T $(KERNEL_DIR)/link.ld

SRC := $(KERNEL_DIR)/src
OBJ := $(KERNEL_DIR)/build

KERNEL_OBJS =  $(patsubst $(SRC)/%.c, $(OBJ)/%.c.o,$(shell find $(SRC) -name '*.c'))
KERNEL_OBJS += $(patsubst $(SRC)/%.S, $(OBJ)/%.S.o,$(shell find $(SRC) -name '*.S'))

DUMMY := $(shell mkdir --parents $(dir $(KERNEL_OBJS)))

.PHONY: kernel_clean

$(KERNEL_OUT): $(KERNEL_DIR)/link.ld ${KERNEL_OBJS}
	@echo [ld] $<
	@${CC} -o $(KERNEL_DIR)/$(KERNEL_OUT) ${KERNEL_OBJS} ${LDFLAGS}

$(OBJ)/%.c.o: $(SRC)/%.c
	@echo [cc] $<
	@${CC} ${DEFINES} ${CFLAGS} -c -o $@ $<

$(OBJ)/%.S.o: $(SRC)/%.S
	@echo [cc] $<
	@${CC} ${DEFINES} ${CFLAGS} -c -o $@ $<

kernel_clean:
	@rm -rf $(OBJ)
	@rm -f $(KERNEL_DIR)/$(KERNEL_OUT)
