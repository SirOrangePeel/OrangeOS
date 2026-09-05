# Multiboot header
.set MAGIC,    0x1BADB002           # For GRUB to use this bootloader
.set FLAGS,    (1<<0 | 1<<1)        # bit 0 = allign modules to page boundaries, bit 1 = provide memory map 
.set CHECKSUM, -(MAGIC + FLAGS)     # Makes sure the three values sum to 0

.section .multiboot
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

.section .text
.extern CallConstructors
.extern KernelMain
.global loader

loader:
    mov $kernel_stack, %esp         # Set stack pointer to kernel stack
    call CallConstructors           # Calls all C++ global/static object constructors
    push %eax                       # Push multiboot magic number
    push %ebx                       # Push to multiboot info structure
    call KernelMain                 # Start the kernel

_stop:
    cli                             # Disable interrupts 
    hlt                             # Halt CPU
    jmp _stop  

# Reserves 2MB of uninitialized memory for kernel
.section .bss
.space 2*1024*1024  
kernel_stack:                       # Top of space