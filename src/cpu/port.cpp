#include "port.h"

Port::Port(uint16_t portNumber) {
    this->portNumber = portNumber;
}
Port::~Port() {}

Port8bit::Port8bit(uint16_t portNumber) : Port(portNumber) {}
Port8bit::~Port8bit() {}

void Port8bit::Write(uint8_t data) {
    __asm__ volatile("outb %0, %1" : : "a" (data), "Nd" (portNumber));
}

uint8_t Port8bit::Read() {
    uint8_t result;
    __asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (portNumber));
    return result;
}

Port8bitSlow::Port8bitSlow(uint16_t portNumber) : Port8bit(portNumber) {}
Port8bitSlow::~Port8bitSlow() {}

// Two jumps after outb act as a short busy-wait for slow legacy hardware (e.g. PIC)
void Port8bitSlow::Write(uint8_t data) {
    __asm__ volatile("outb %0, %1\njmp 1f\njmp 1f\n1:" : : "a" (data), "Nd" (portNumber));
}

Port16bit::Port16bit(uint16_t portNumber) : Port(portNumber) {}
Port16bit::~Port16bit() {}

void Port16bit::Write(uint16_t data) {
    __asm__ volatile("outw %0, %1" : : "a" (data), "Nd" (portNumber));
}

uint16_t Port16bit::Read() {
    uint16_t result;
    __asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" (portNumber));
    return result;
}

Port32bit::Port32bit(uint16_t portNumber) : Port(portNumber) {}
Port32bit::~Port32bit() {}

void Port32bit::Write(uint32_t data) {
    __asm__ volatile("outl %0, %1" : : "a" (data), "Nd" (portNumber));
}

uint32_t Port32bit::Read() {
    uint32_t result;
    __asm__ volatile("inl %1, %0" : "=a" (result) : "Nd" (portNumber));
    return result;
}