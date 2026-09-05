.set IRQ_BASE, 0x20   # Constant -- IRQ 0 maps to interrupt vector 0x20

.section .text
.extern _ZN16InterruptManager15HandleInterruptEhj         
.global _ZN16InterruptManager22IgnoreInterruptRequestEv   

# Macros for generating asm stub for each IRQ
.macro HandleException num
.global _ZN16InterruptManager16HandleException\num\()Ev
_ZN16InterruptManager16HandleException\num\()Ev:
    movb $\num, (interruptNumber)
    jmp int_bottom
.endm

.macro HandleInterruptRequest num
.global _ZN16InterruptManager26HandleInterruptRequest\num\()Ev
_ZN16InterruptManager26HandleInterruptRequest\num\()Ev:
    movb $(\num + IRQ_BASE), (interruptNumber)
    jmp int_bottom
.endm

HandleInterruptRequest 0x00   
HandleInterruptRequest 0x01   
HandleInterruptRequest 0x0C   

int_bottom:
    pusha           # Save all general purpose registers     
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs       # Save segment registers 

    push %esp                                       # Pass current stack pointer as argument                                    
    push (interruptNumber)                          # Pass interrupt number as argument
    call _ZN16InterruptManager15HandleInterruptEhj  # Call C++ HandlePointer
    add $8, %esp                                    # Clean up pushed arguments
    movl %eax, %esp                                 # Use return value as new stack pointer

    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa            # Restore all registers

_ZN16InterruptManager22IgnoreInterruptRequestEv:
    iret            # Return from interrupt

.data
interruptNumber: .byte 0