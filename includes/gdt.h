#ifndef __GDT_H
#define __GDT_H
    #include "types.h"

    class GlobalDescriptorTable {
    public:
        class SegmentDescriptor {
            private:
                uint16_t limit_lo;          // Low 16 bits of segment size
                uint16_t base_lo;           // Low 12 bits of segment start address
                uint8_t  base_hi;           // Next 8 bits of segment start address
                uint8_t  type;              // Access flags
                uint8_t  flags_limit_hi;    // Upper 4 bits of limit + granularity flags
                uint8_t  base_vhi;          // High 8 bits of segment start address
            public:
                SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t type);
                uint32_t Base();
                uint32_t Limit();
        } __attribute__((packed)); // No padding

    private:
        // Segments
        SegmentDescriptor nullSegmentSelector;      // Required by CPU, always zero
        SegmentDescriptor unusedSegmentSelector;    // padding/placeholder
        SegmentDescriptor codeSegmentSelector;      // Executable data
        SegmentDescriptor dataSegmentSelector;      // Read/Wrtie data

    public:
        GlobalDescriptorTable();
        ~GlobalDescriptorTable();

        // Byte offsets for each descriptor 
        uint16_t CodeSegmentSelector();  
        uint16_t DataSegmentSelector();  
    };

#endif