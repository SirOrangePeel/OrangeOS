#ifndef __GDT_H
#define __GDT_H
#include "types.h"

// Manages the x86 Global Descriptor Table (GDT) for memory segmentation
class GlobalDescriptorTable {
public:
    // Represents a single 8-byte GDT entry describing a memory segment
    class SegmentDescriptor {
        private:
            uint16_t limit_lo;       // Lower 16 bits of segment limit
            uint16_t base_lo;        // Lower 16 bits of base address
            uint8_t  base_hi;        // Bits 16-23 of base address
            uint8_t  type;           // Segment type and access flags
            uint8_t  flags_limit_hi; // Upper 4 bits of limit + granularity flags
            uint8_t  base_vhi;       // Bits 24-31 of base address
        public:
            SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t type);
            uint32_t Base();
            uint32_t Limit();
    } __attribute__((packed)); // Prevent compiler padding

private:
    SegmentDescriptor nullSegmentSelector;    // Required null descriptor (index 0)
    SegmentDescriptor unusedSegmentSelector;  // Unused placeholder descriptor
    SegmentDescriptor codeSegmentSelector;    // Executable code segment
    SegmentDescriptor dataSegmentSelector;    // Read/write data segment

public:
    GlobalDescriptorTable();
    ~GlobalDescriptorTable();
    uint16_t CodeSegmentSelector();  // Returns byte offset of code segment
    uint16_t DataSegmentSelector();  // Returns byte offset of data segment
};

#endif