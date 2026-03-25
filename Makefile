# Compiler/Assembler/Linker flags
GPPPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore
ASPARAMS = --32
LDPARAMS = -melf_i386

# Directories
SRC_DIR = src
BIN_DIR = bin

# Recursively find all source files
ASM_SOURCES := $(shell find $(SRC_DIR) -name '*.s')
CPP_SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')

# Generate object file paths (flatten all sources into bin/)
OBJECTS := $(patsubst $(SRC_DIR)/%.s, $(BIN_DIR)/%.o, $(ASM_SOURCES)) \
           $(patsubst $(SRC_DIR)/%.cpp, $(BIN_DIR)/%.o, $(CPP_SOURCES))

# Preserve subdirectory structure under bin/
$(BIN_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(dir $@)
	as $(ASPARAMS) -o $@ $<

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	g++ $(GPPPARAMS) -o $@ -I includes -c $<

# Building without running
all: kernel.iso

# Link kernel binary
kernel.bin: linker.ld $(OBJECTS)
	ld $(LDPARAMS) -T $< -o $@ $(OBJECTS)

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
	@if lsmod | grep -q "^kvm"; then \
		bash toggle_kvm_for_vbox.sh; \
	fi
	(killall VirtualBoxVM && sleep 1) || true
	VirtualBoxVM --startvm "Orange OS" &

# Ensure bin/ exists before building
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf kernel.bin kernel.iso
	rm -rf $(BIN_DIR)

.PHONY: clean install run all