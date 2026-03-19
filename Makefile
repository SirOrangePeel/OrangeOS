# Toolchain
CC = i686-elf-gcc
AS = i686-elf-as
LD = i686-elf-gcc

# Flags
CFLAGS  = -c -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -T linker.ld -ffreestanding -O2 -nostdlib -lgcc

# Directories
BOOT_SRC  = boot/src
BOOT_BIN  = build/boot/bin
KERNEL_SRC = kernel/src
KERNEL_BIN = build/kernel/bin
BUILD_DIR  = build
ISO_DIR = iso/boot/grub

# Source & Object Files
BOOT_SRC_FILES   = $(wildcard $(BOOT_SRC)/*.s)
KERNEL_SRC_FILES = $(wildcard $(KERNEL_SRC)/*.c)

BOOT_OBJ   = $(patsubst $(BOOT_SRC)/%.s,   $(BOOT_BIN)/%.o,   $(BOOT_SRC_FILES))
KERNEL_OBJ = $(patsubst $(KERNEL_SRC)/%.c, $(KERNEL_BIN)/%.o, $(KERNEL_SRC_FILES))

# Final Output
OS_IMAGE = $(BUILD_DIR)/OrangeOS.bin
ISO_IMAGE = $(BUILD_DIR)/OrangeOS.iso

# Rules
all: $(OS_IMAGE) verify iso

$(OS_IMAGE): $(BOOT_OBJ) $(KERNEL_OBJ) | $(BUILD_DIR)
	$(LD) $(LDFLAGS) $(BOOT_OBJ) $(KERNEL_OBJ) -o $@

$(BOOT_BIN)/%.o: $(BOOT_SRC)/%.s | $(BOOT_BIN)
	$(AS) $< -o $@

$(KERNEL_BIN)/%.o: $(KERNEL_SRC)/%.c | $(KERNEL_BIN)
	$(CC) $(CFLAGS) $< -o $@

$(BOOT_BIN):
	mkdir -p $(BOOT_BIN)

$(KERNEL_BIN):
	mkdir -p $(KERNEL_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

iso: $(OS_IMAGE)
	mkdir -p isodir/boot/grub
	cp $(OS_IMAGE) isodir/boot/OrangeOS.bin
	cp iso/boot/grub/grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO_IMAGE) isodir
	rm -rf isodir

verify: $(OS_IMAGE)
	@if grub-file --is-x86-multiboot $(OS_IMAGE); then \
		echo "multiboot confirmed"; \
	else \
		echo "error: $(OS_IMAGE) is not multiboot"; \
		exit 1; \
	fi

run: $(ISO_IMAGE)
	qemu-system-i386 -cdrom $(ISO_IMAGE)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean verify run