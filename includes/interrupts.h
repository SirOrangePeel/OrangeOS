#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H
#include "types.h"
#include "port.h"
#include "gdt.h"

class InterruptManager;

class InterruptHandler {
    protected:
        uint8_t interruptNumber;
        InterruptManager* interruptManager;

        InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager);
        ~InterruptHandler();

    public:
        virtual uint32_t HandleInterrupt(uint32_t esp);
};

// Manages the x86 Interrupt Descriptor Table (IDT) and PIC initialisation
class InterruptManager {
    friend class InterruptHandler;

    protected:

        static InterruptManager* ActiveInterruptManager;
        InterruptHandler* handlers[256];

        // Single 8-byte IDT entry pointing to an interrupt handler
        struct GateDescriptor {
            uint16_t handlerAddress_lo;         // Lower 16 bits of handler address
            uint16_t gdtCodeSegmentSelector;    // GDT selector for the handler's code segment
            uint8_t  reserved;                  // Must be zero
            uint8_t  access;                    // Present, privilege level, and gate type flags
            uint16_t handlerAddress_hi;         // Upper 16 bits of handler address
        } __attribute__((packed));

        static GateDescriptor interruptDescriptorTable[256]; // The full 256-entry IDT

        // Loaded into the IDTR register via lidt
        struct InterruptDescriptorTablePointer {
            uint16_t size;  // Size of IDT in bytes minus 1
            uint32_t base;  // Linear address of IDT
        } __attribute__((packed));

        // Writes a single entry into the IDT
        static void SetInterruptDescriptorTableEntry(
            uint8_t  interruptNumber,
            uint16_t gdtCodeSegmentOffset,
            void     (*handler)(),
            uint8_t  DescriptorPriviledgeLevel,
            uint8_t  DescriptorType
        );

        // I/O ports for the master and slave 8259 PICs
        Port8bitSlow picMasterCommand;
        Port8bitSlow picMasterData;
        Port8bitSlow picSlaveCommand;
        Port8bitSlow picSlaveData;

    public:
        InterruptManager(GlobalDescriptorTable* gdt);
        ~InterruptManager();
        void Activate(); // Enables interrupts (sti)
        void Deactivate();

        static uint32_t HandleInterrupt(uint8_t interruptNumber, uint32_t esp);
        uint32_t DoHandleInterrupt(uint8_t interruptNumber, uint32_t esp);
        static void IgnoreInterruptRequest();       // Default no-op handler
        static void HandleInterruptRequest0x00();   // IRQ0 – timer
        static void HandleInterruptRequest0x01();   // IRQ1 – keyboard
};

#endif