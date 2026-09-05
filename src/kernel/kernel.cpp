#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "driver.h"
#include "keyboard.h"
#include "mouse.h"

void printf(char* data) {
    // MMIO address
    static uint16_t* videoMemory = (uint16_t*)0xB8000;

    // Inital print position
    static uint8_t x = 0, y = 0;

    for(int i = 0; data[i] != '\0'; ++i) {
        switch(data[i]) {
            case '\n':      // Create new line
                y++;
                x = 0;     
                break;           
            default:        // & 0xFF00 perserves color attribute, | data[i] sets character byte
                videoMemory[80*y+x] = (videoMemory[80*y+x] & 0xFF00) | data[i];
                x++;
        }

        // Line wrap when out of space on row
        if(x >= 80) {
            y++;
            x = 0;
        }

        // Clear screen and position x, y back at top when out of rows
        if (y >= 25) {
            for(y = 0; y < 25; y++) {
                for(x = 0; x < 80; x++) {
                    videoMemory[80*y+x] = (videoMemory[80*y+x] & 0xFF00) | ' ';
                }
            }
            x = 0;
            y = 0;
        }
    }
}

typedef void (*constructor)();

// Linker symbols marking start and end of .ctors section
// .ctors is a list of function pointers to all C++ constructors
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

// Call each constructor
extern "C" void CallConstructors() {
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

/* Main entry point
*    Order matters: 
*       1. GDT must exist before interrupt manager
*       2. Drivers must register before interrupts are activated
*/
extern "C" void KernelMain(void* multibootStructure, uint32_t /* magicNumber */) {
    printf("Hello World!\n");

    // Initializing tables and interrupts
    GlobalDescriptorTable gdt;
    InterruptManager interrupts(&gdt);

    DriverManager drvManager;

        KeyboardDriver KeyboardDriver(&interrupts);
        //drvManager.AddDriver(KeyboardDriver);

        MouseDriver MouseDriver(&interrupts);
        //drvManager.AddDriver(MouseDriver);

    interrupts.Activate();

    while(1);
}