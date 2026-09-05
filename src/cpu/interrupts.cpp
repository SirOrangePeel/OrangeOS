#include "interrupts.h"
void printf(char*);

InterruptHandler::InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager) {
    this->interruptNumber = interruptNumber;
    this->interruptManager = interruptManager;
    interruptManager->handlers[interruptNumber] = this;
}

InterruptHandler::~InterruptHandler() {
    if(interruptManager->handlers[interruptNumber] == this)
        interruptManager->handlers[interruptNumber] = 0;
}

uint32_t InterruptHandler::HandleInterrupt(uint32_t esp) {
    return esp;
}

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[TOTAL_INTERRUPTS];
InterruptManager* InterruptManager::ActiveInterruptManager = 0;

void InterruptManager::SetInterruptDescriptorTableEntry(
    uint8_t  interruptNumber,
    uint16_t gdtCodeSegmentSelectorOffset, 
    void     (*handler)(),
    uint8_t  DescriptorPriviledgeLevel,
    uint8_t  DescriptorType)
{
    const uint8_t IDT_DESC_PRESENT = 0x80;
    interruptDescriptorTable[interruptNumber].handlerAddress_lo = ((uint32_t)handler) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].handlerAddress_hi = (((uint32_t)handler) >> 16) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].gdtCodeSegmentSelector = gdtCodeSegmentSelectorOffset;
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

    // Set all IDT entries to point to ignore by default
    for (uint16_t i = 0; i < TOTAL_INTERRUPTS; i++) {
        handlers[i] = 0;
        SetInterruptDescriptorTableEntry(i, CodeSegment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);
    }

    // Installs real handlers for Timer, Keyboard, and Mouse
    SetInterruptDescriptorTableEntry(0x20, CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x21, CodeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x2C, CodeSegment, &HandleInterruptRequest0x0C, 0, IDT_INTERRUPT_GATE);

    // Initallize PIC for reconfiguration
    picMasterCommand.Write(0x11);
    picSlaveCommand.Write(0x11);

    // Remaps PICs so interrupts start at 0x20
    picMasterData.Write(0x20);      
    picSlaveData.Write(0x28);       

    // Configures how the slave PIC is connected to Master
    picMasterData.Write(0x04);      
    picSlaveData.Write(0x02);       

    // Sets 8086 mode
    picMasterData.Write(0x01);     
    picSlaveData.Write(0x01);

    // Unmasks all interrupt requests (IRQs)
    picMasterData.Write(0x00);      
    picSlaveData.Write(0x00);       

    // Loads IDT into memory 
    InterruptDescriptorTablePointer idt;
    idt.size = 256 * sizeof(GateDescriptor) - 1;
    idt.base = (uint32_t)interruptDescriptorTable;
    asm volatile("lidt %0" : : "m" (idt));
}

InterruptManager::~InterruptManager() {
    Deactivate();
}

void InterruptManager::Deactivate() {
    if(ActiveInterruptManager == this) {
        ActiveInterruptManager = 0;
        asm("cli");
    }
}

// Enable hardware interrupts
void InterruptManager::Activate() {
    if(ActiveInterruptManager != 0)
        ActiveInterruptManager->Deactivate();

    ActiveInterruptManager = this;
    asm("sti");
}

uint32_t InterruptManager::HandleInterrupt(uint8_t interruptNumber, uint32_t esp) {
    if(ActiveInterruptManager != 0) 
        return ActiveInterruptManager->DoHandleInterrupt(interruptNumber, esp);
    return esp;
}

/*
*   Look up to see if driver registered itself for this interrupt
*   Ignores nuhandled timer interrupt because 0x20 fires constantly, would spam the screen
*/
uint32_t InterruptManager::DoHandleInterrupt(uint8_t interruptNumber, uint32_t esp) {
    if(handlers[interruptNumber] != 0) {
        esp = handlers[interruptNumber]->HandleInterrupt(esp);
    } else if (interruptNumber != 0x20) {
        char* interruptMessage = "UNHANDLED INTERRUPT 0x00\n";
        char* hex = "0123456789ABCDEF";
        interruptMessage[22] = hex[(interruptNumber >> 4) & 0x0F];
        interruptMessage[23] = hex[interruptNumber & 0x0F];
        printf(interruptMessage);
    }

    // Send End of Interrupt value back to PIC
    if(0x20 <= interruptNumber && interruptNumber < 0x30) {
        picMasterCommand.Write(0x20);

        if(0x28 <= interruptNumber)
            picSlaveCommand.Write(0x20);
    }

    return esp;
}