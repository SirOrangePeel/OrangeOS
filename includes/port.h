#ifndef __PORT_H
#define __PORT_H
    #include "types.h"

    // Base class
    class Port {
    protected:
        uint16_t portNumber;
        Port(uint16_t portNumber);
        ~Port();
    };

    // Send 1 byte
    class Port8bit : public Port {
    public:
        Port8bit(uint16_t portNumber);
        ~Port8bit();
        virtual void    Write(uint8_t data);
        virtual uint8_t Read();
    };

    // Sends 1 byte, in 2 steps to allow hardware to catch up
    class Port8bitSlow : public Port8bit {
    public:
        Port8bitSlow(uint16_t portNumber);
        ~Port8bitSlow();
        virtual void Write(uint8_t data);
    };

    // Sends 2 bytes
    class Port16bit : public Port {
    public:
        Port16bit(uint16_t portNumber);
        ~Port16bit();
        virtual void     Write(uint16_t data);
        virtual uint16_t Read();
    };

    // Sends 4 bytes
    class Port32bit : public Port {
    public:
        Port32bit(uint16_t portNumber);
        ~Port32bit();
        virtual void     Write(uint32_t data);
        virtual uint32_t Read();
    };

#endif