#include "mouse.h"
void printf(char*);


MouseDriver::MouseDriver(InterruptManager* manager) 
: InterruptHandler(0x2C, manager),
  dataPort(0x60),
  commandPort(0x64) {

    offset = 0;
    buttons = 0;

    static uint16_t* videoMemory = (uint16_t*)0xB8000;
    videoMemory[80*12+40] = ((videoMemory[80*12+40] & 0xF000) >> 4) | ((videoMemory[80*12+40] & 0x0F00) << 4) | (videoMemory[80*12+40] & 0x00FF);

    commandPort.Write(0x20);                // Get current config byte
    uint8_t status = dataPort.Read() | 2;   // Read config byte back
    commandPort.Write(0x60);                // Initalizes command port
    dataPort.Write(status);                 // Writes modded config bytes, activates IRQ12

    commandPort.Write(0xD4);                // Makes sure next byte goes to mouse
    dataPort.Write(0xF4);                   // Enables mouse data reporting
    dataPort.Read();                        // Discards mouse acknowledgement
}

MouseDriver::~MouseDriver() {}

uint32_t MouseDriver::HandleInterrupt(uint32_t esp) {
    // Check bit 5 just incase a different byte got here
    uint8_t status = commandPort.Read();
    if(!(status & 0x20))
        return esp;

    static int8_t x = 40, y = 12;

    // Mouse sends data in 3-byte packets
    buffer[offset] = dataPort.Read();
    offset = (offset + 1) % 3;

    // If full packet received, change mouse position
    if(offset == 0) {
        static uint16_t* videoMemory = (uint16_t*)0xB8000;

        videoMemory[80*y+x] = ((videoMemory[80*y+x] & 0xF000) >> 4) | ((videoMemory[80*y+x] & 0x0F00) << 4) | (videoMemory[80*y+x] & 0x00FF);

        x += buffer[1];         // Buffer[1] = X movement delta
        if (x < 0) x = 0;
        if (x > 80) x = 79;


        y -= buffer[2];         // Buffer[2] = Y movement delta
        if (y < 0) x = 0;
        if (y > 25) x = 24;

        videoMemory[80*y+x] = ((videoMemory[80*y+x] & 0xF000) >> 4) | ((videoMemory[80*y+x] & 0x0F00) << 4) | (videoMemory[80*y+x] & 0x00FF);

        for(uint8_t i = 0; i < 3; i++) {
            if((buffer[0] & (0x01 << i)) != (buttons & (0x01 << i))) {
                videoMemory[80*y+x] = ((videoMemory[80*y+x] & 0xF000) >> 4) | ((videoMemory[80*y+x] & 0x0F00) << 4) | (videoMemory[80*y+x] & 0x00FF);
            }

        }
        buttons = buffer[0];
    }
    
    return esp;
}