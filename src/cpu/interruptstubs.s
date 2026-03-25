.set IRQ_BASE, 0x20   # Hardware IRQs are remapped to vectors 0x20-0x2F by the PIC

.section .text
.extern _ZN16InterruptManager15handleInterruptEhj          # InterruptManager::handleInterrupt()
.global _ZN16InterruptManager22IgnoreInterruptRequestEv    # InterruptManager::IgnoreInterruptRequest()

# Macro for CPU exceptions: stores the exception number and jumps to common handler
.macro HandleException num
.global _ZN16InterruptManager16HandleException\num\()Ev
_ZN16InterruptManager16HandleException\num\()Ev:
    movb $\num, (interruptNumber)
    jmp int_bottom
.endm

# Macro for hardware IRQs: offsets the IRQ number by IRQ_BASE before jumping to common handler
.macro HandleInterruptRequest num
.global _ZN16InterruptManager26HandleInterruptRequest\num\()Ev
_ZN16InterruptManager26HandleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptNumber)
    jmp int_bottom
.endm

HandleInterruptRequest 0x00   # IRQ0 – timer
HandleInterruptRequest 0x01   # IRQ1 – keyboard

# Common interrupt handler: saves full CPU state, calls C++ handler, then restores
int_bottom:
    pusha               # Save general-purpose registers
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs

    push %esp                                        # Pass current stack pointer as argument
    push (interruptNumber)                           # Pass interrupt number as argument
    call _ZN16InterruptManager15handleInterruptEhj  # Returns (possibly new) esp in %eax
    movl %eax, %esp                                  # Switch to returned stack pointer

    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa                # Restore general-purpose registers

# IgnoreInterruptRequest falls through to here — just return from interrupt
_ZN16InterruptManager22IgnoreInterruptRequestEv:
    iret

.data
interruptNumber: .byte 0   # Scratch variable shared between stubs and int_bottom