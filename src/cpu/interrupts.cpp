#include "interrupts.h"
void printf(char* data);

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

// Fills one IDT entry with the handler address, segment selector, and access flags
void InterruptManager::SetInterruptDescriptorTableEntry(
    uint8_t  interruptNumber,
    uint16_t gdtCodeSegmentOffset,
    void     (*handler)(),
    uint8_t  DescriptorPriviledgeLevel,
    uint8_t  DescriptorType)
{
    const uint8_t IDT_DESC_PRESENT = 0x80;
    interruptDescriptorTable[interruptNumber].handlerAddress_lo = ((uint32_t)handler) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].handlerAddress_hi = (((uint32_t)handler) >> 16) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].gdtCodeSegmentSelector = gdtCodeSegmentOffset;
    // Present bit | DPL (2 bits) | gate type
    interruptDescriptorTable[interruptNumber].access = IDT_DESC_PRESENT | ((DescriptorPriviledgeLevel & 3) << 5) | DescriptorType;
    interruptDescriptorTable[interruptNumber].reserved = 0;
}

InterruptManager::InterruptManager(GlobalDescriptorTable* gdt)
    : picMasterCommand(0x20),
      picMasterData(0x21),
      picSlaveCommand(0xA0),
      picSlaveData(0xA1)
{
    uint16_t CodeSegment = gdt->CodeSegmentSelector();
    const uint8_t IDT_INTERRUPT_GATE = 0xE;

    // Default all 256 entries to the no-op ignore handler
    for (uint16_t i = 0; i < 256; i++)
        SetInterruptDescriptorTableEntry(i, CodeSegment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);

    // Install handlers for timer (IRQ0 -> 0x20) and keyboard (IRQ1 -> 0x21)
    SetInterruptDescriptorTableEntry(0x20, CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x21, CodeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);

    // Initialise master and slave PICs (ICW1-ICW4)
    picMasterCommand.Write(0x11);   // ICW1: start initialisation
    picSlaveCommand.Write(0x11);
    picMasterData.Write(0x20);      // ICW2: master IRQs mapped to vectors 0x20-0x27
    picSlaveData.Write(0x28);       // ICW2: slave IRQs mapped to vectors 0x28-0x2F
    picMasterData.Write(0x04);      // ICW3: slave connected to master IRQ2
    picSlaveData.Write(0x02);       // ICW3: slave cascade identity
    picMasterData.Write(0x01);      // ICW4: 8086 mode
    picSlaveData.Write(0x01);
    picMasterData.Write(0x00);      // Unmask all master IRQs
    picSlaveData.Write(0x00);       // Unmask all slave IRQs

    // Load the IDT register
    InterruptDescriptorTablePointer idt;
    idt.size = 256 * sizeof(GateDescriptor) - 1;
    idt.base = (uint32_t)interruptDescriptorTable;
    asm volatile("lidt %0" : : "m" (idt));
}

InterruptManager::~InterruptManager() {
}

// Enable hardware interrupts
void InterruptManager::Activate() {
    asm("sti");
}

uint32_t InterruptManager::handleInterrupt(uint8_t interruptNumber, uint32_t esp) {
    printf(" INTERRUPT ");
    return esp; // Return (possibly modified) stack pointer
}