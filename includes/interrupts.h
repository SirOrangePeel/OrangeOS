#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H
    #include "types.h"
    #include "port.h"
    #include "gdt.h"
    #define TOTAL_INTERRUPTS 256

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

    class InterruptManager {
        friend class InterruptHandler;

        protected:

            static InterruptManager* ActiveInterruptManager;
            InterruptHandler* handlers[TOTAL_INTERRUPTS];

            // Handler address is split into 2 halves, for backwards compatibility
            struct GateDescriptor {
                uint16_t handlerAddress_lo;     
                uint16_t gdtCodeSegmentSelector;
                uint8_t  reserved;                  
                uint8_t  access;                    
                uint16_t handlerAddress_hi;         
            } __attribute__((packed));

            static GateDescriptor interruptDescriptorTable[TOTAL_INTERRUPTS];

            struct InterruptDescriptorTablePointer {
                uint16_t size; 
                uint32_t base;  
            } __attribute__((packed));

            static void SetInterruptDescriptorTableEntry(
                uint8_t  interruptNumber,
                uint16_t gdtCodeSegmentOffset,
                void     (*handler)(),
                uint8_t  DescriptorPriviledgeLevel,
                uint8_t  DescriptorType
            );

            // 4 ports for talking to the 2 PIC chips
            // Uses slow because PIC needs small delay
            Port8bitSlow picMasterCommand;
            Port8bitSlow picMasterData;
            Port8bitSlow picSlaveCommand;
            Port8bitSlow picSlaveData;

        public:
            InterruptManager(GlobalDescriptorTable* gdt);
            ~InterruptManager();
            void Activate(); 
            void Deactivate();

            static uint32_t HandleInterrupt(uint8_t interruptNumber, uint32_t esp);
            uint32_t DoHandleInterrupt(uint8_t interruptNumber, uint32_t esp);
            static void IgnoreInterruptRequest();       
            static void HandleInterruptRequest0x00();   
            static void HandleInterruptRequest0x01();   
            static void HandleInterruptRequest0x0C();   
    };

#endif