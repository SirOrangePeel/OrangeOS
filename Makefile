# Compiler/Assembler/Linker flags
GPPPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore
ASPARAMS = --32
LDPARAMS = -melf_i386

# Directories
SRC_DIR = src
BIN_DIR = bin

# Sources and objects
objects = $(BIN_DIR)/loader.o $(BIN_DIR)/kernel.o $(BIN_DIR)/gdt.o $(BIN_DIR)/port.o

# Compile C++ source files
$(BIN_DIR)/%.o: $(SRC_DIR)/bootloader/%.s
	as $(ASPARAMS) -o $@ $<

$(BIN_DIR)/%.o: $(SRC_DIR)/kernel/%.cpp
	g++ $(GPPPARAMS) -o $@ -I includes -c $<

# Link kernel binary
kernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

# Install to /boot
install: kernel.bin
	sudo cp $< /boot/kernel.bin

# Build bootable ISO
kernel.iso: kernel.bin
	mkdir -p iso/boot/grub
	cp $< iso/boot
	@echo 'set timeout=0'                    >> iso/boot/grub/grub.cfg
	@echo 'set default=0'                    >> iso/boot/grub/grub.cfg
	@echo 'menuentry "Orange OS" {'          >> iso/boot/grub/grub.cfg
	@echo '  multiboot /boot/kernel.bin'     >> iso/boot/grub/grub.cfg
	@echo '  boot'                           >> iso/boot/grub/grub.cfg
	@echo '}'                                >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf iso

# Run in VirtualBox
run: kernel.iso
	(killall VirtualBoxVM && sleep 1) || true
	VirtualBoxVM --startvm "Orange OS" &

# Ensure bin/ exists before building
$(objects): | $(BIN_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf build/kernel.bin build/kernel.iso
	rm -rf $(BIN_DIR)

.PHONY: clean install run
