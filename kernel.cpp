void printf(char* data) {
    static unsigned short* videoMemory = (unsigned short*)0xB8000;

    for(int i = 0; data[i] != '\0'; ++i) {
        videoMemory[i] = (videoMemory[i] & 0xFF00) | data[i];
    }
}

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

extern "C" void CallConstructors() {
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

extern "C" void KernelMain(void* multibootStructure, unsigned int magicNumber) {
    printf("Hello World!");

    while(1);
}