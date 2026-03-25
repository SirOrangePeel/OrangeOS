#ifndef __PORT_H
#define __PORT_H
#include "types.h"

// Base class holding the port number for all port width variants
class Port {
protected:
    uint16_t portNumber;
    Port(uint16_t portNumber);
    ~Port();
};

// 8-bit x86 I/O port access via inb/outb
class Port8bit : public Port {
public:
    Port8bit(uint16_t portNumber);
    ~Port8bit();
    virtual void    Write(uint8_t data);
    virtual uint8_t Read();
};

// Slower 8-bit port write with two short jumps after outb to introduce a delay
class Port8bitSlow : public Port8bit {
public:
    Port8bitSlow(uint16_t portNumber);
    ~Port8bitSlow();
    virtual void Write(uint8_t data);
};

// 16-bit x86 I/O port access via inw/outw
class Port16bit : public Port {
public:
    Port16bit(uint16_t portNumber);
    ~Port16bit();
    virtual void     Write(uint16_t data);
    virtual uint16_t Read();
};

// 32-bit x86 I/O port access via inl/outl
class Port32bit : public Port {
public:
    Port32bit(uint16_t portNumber);
    ~Port32bit();
    virtual void     Write(uint32_t data);
    virtual uint32_t Read();
};

#endif