# Multiboot magic values — identify this as a bootable kernel image to GRUB
.set MAGIC,    0x1BADB002
.set FLAGS,    (1<<0 | 1<<1)   # Flags: align modules on page boundaries, provide memory map
.set CHECKSUM, -(MAGIC + FLAGS) # Must sum to zero with MAGIC and FLAGS

.section .multiboot
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

.section .text
.extern CallConstructors  # Runs global C++ constructors before main
.extern KernelMain
.global loader

loader:
    mov $kernel_stack, %esp   # Set up the initial kernel stack
    call CallConstructors
    push %eax                 # Push multiboot magic number (passed by bootloader)
    push %ebx                 # Push multiboot info struct pointer (passed by bootloader)
    call KernelMain

_stop:
    cli        # Disable interrupts
    hlt        # Halt the CPU
    jmp _stop  # Safety loop in case of spurious wake-up

.section .bss
.space 2*1024*1024  # 2 MiB kernel stack space
kernel_stack: