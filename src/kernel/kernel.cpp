#include "types.h"
#include "gdt.h"

void printf(char* data) {
    static uint16_t* videoMemory = (uint16_t*)0xB8000;

    static uint8_t x = 0, y = 0;

    for(int i = 0; data[i] != '\0'; ++i) {
        switch(data[i]) {
            case '\n':
                y++;
                x = 0;     
                break;           
            default:
                videoMemory[80*y+x] = (videoMemory[80*y+x] & 0xFF00) | data[i];
                x++;
        }

        if(x >= 80) {
            y++;

            x = 0;
        }

        if (y >= 25) {
            for(y = 0; y < 25; y++) {
                for(x = 0; x < 25; x++) {
                    videoMemory[80*y+x] = (videoMemory[80*y+x] & 0xFF00) | ' ';
                }
            }
            x = 0;
            y = 0;
        }
    }
}

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

extern "C" void CallConstructors() {
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

extern "C" void KernelMain(void* multibootStructure, uint32_t /* magicNumber */) {
    printf("Hello World!\n");

    GlobalDescriptorTable gdt;

    while(1);
}