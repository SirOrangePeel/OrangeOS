#include "keyboard.h"
void printf(char*);

char KeyboardDriver::scancodeToASCII[] = {
    0,    // 0x00 - error
    0,    // 0x01 - Escape
    '1',  // 0x02
    '2',  // 0x03
    '3',  // 0x04
    '4',  // 0x05
    '5',  // 0x06
    '6',  // 0x07
    '7',  // 0x08
    '8',  // 0x09
    '9',  // 0x0A
    '0',  // 0x0B
    '-',  // 0x0C
    '=',  // 0x0D
    0,    // 0x0E - Backspace
    0,    // 0x0F - Tab
    'q',  // 0x10
    'w',  // 0x11
    'e',  // 0x12
    'r',  // 0x13
    't',  // 0x14
    'y',  // 0x15
    'u',  // 0x16
    'i',  // 0x17
    'o',  // 0x18
    'p',  // 0x19
    '[',  // 0x1A
    ']',  // 0x1B
    0,    // 0x1C - Enter
    0,    // 0x1D - Left Ctrl
    'a',  // 0x1E
    's',  // 0x1F
    'd',  // 0x20
    'f',  // 0x21
    'g',  // 0x22
    'h',  // 0x23
    'j',  // 0x24
    'k',  // 0x25
    'l',  // 0x26
    ';',  // 0x27
    '\'', // 0x28
    '`',  // 0x29
    0,    // 0x2A - Left Shift
    '\\', // 0x2B
    'z',  // 0x2C
    'x',  // 0x2D
    'c',  // 0x2E
    'v',  // 0x2F
    'b',  // 0x30
    'n',  // 0x31
    'm',  // 0x32
    ',',  // 0x33
    '.',  // 0x34
    '/',  // 0x35
    0,    // 0x36 - Right Shift
    0,    // 0x37 - (keypad *)
    0,    // 0x38 - Left Alt
    ' ',  // 0x39 - Space
};

char KeyboardDriver::scancodeToASCIIShifted[] = {
    0,      // 0x00 - Error
    0,      // 0x01 - Escape
    '!',    // 0x02
    '@',    // 0x03
    '#',    // 0x04
    '$',    // 0x05
    '%',    // 0x06
    '^',    // 0x07
    '&',    // 0x08
    '*',    // 0x09
    '(',    // 0x0A
    ')',    // 0x0B
    '_',    // 0x0C
    '+',    // 0x0D
    0,      // 0x0E - Backspace
    0,      // 0x0F - Tab
    'Q',    // 0x10
    'W',    // 0x11
    'E',    // 0x12
    'R',    // 0x13
    'T',    // 0x14
    'Y',    // 0x15
    'U',    // 0x16
    'I',    // 0x17
    'O',    // 0x18
    'P',    // 0x19
    '{',    // 0x1A
    '}',    // 0x1B
     0,     // 0x1C - Enter
     0,     // 0x1D - Left Ctrl
    'A',    // 0x1E
    'S',    // 0x1F
    'D',    // 0x20
    'F',    // 0x21
    'G',    // 0x22
    'H',    // 0x23
    'J',    // 0x24
    'K',    // 0x25
    'L',    // 0x26
    ':',    // 0x27
    '"',    // 0x28
    '~',    // 0x29
    0,      // 0x2A - Left Shift
    '|',    // 0x2B
    'Z',    // 0x2C
    'X',    // 0x2D
    'C',    // 0x2E
    'V',    // 0x2F
    'B',    // 0x30
    'N',    // 0x31
    'M',    // 0x32
    '<',    // 0x33
    '>',    // 0x34
    '?',    // 0x35
    0,      // 0x36 - Right Shift
    0,      // 0x37 - (keypad *)
    0,      // 0x38 - Left Alt
    ' ',    // 0x39 - Space
};


KeyboardDriver::KeyboardDriver(InterruptManager* manager) 
: InterruptHandler(0x21, manager),
  dataPort(0x60),
  commandPort(0x64) {
    while(commandPort.Read() & 0x1)
        dataPort.Read();

    commandPort.Write(0xAE);    // Activates Keyboard interrupts
    commandPort.Write(0x20);    // Get current state
    uint8_t status = (dataPort.Read() | 1) & ~0x10; 
    commandPort.Write(0x60);    // Set current state
    dataPort.Write(status);     // Write current state back

    dataPort.Write(0xF4);       // Activate Keyboard (I think)
}

KeyboardDriver::~KeyboardDriver() {}

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp) {
    uint8_t key = dataPort.Read();

    // Handle key-up events first (scancodes >= 0x80)
    if(key & 0x80) {
        uint8_t released = key & 0x7F;
        if(released == 0x2A || released == 0x36)
            shift = false;

        return esp;
    }

    //Key-Down events
    switch(key) {
        case 0x2A: case 0x36:   // Letf/Right shift
            shift = true;
            break;
        case 0x3A:              // Capslock
            capslock = !capslock;
            break;
        case 0x1C:              // Enter
            printf("\n");
            break;
        case 0x0E:              // Backspace
            // Not handled yet
            break;
        default:
            if(key < sizeof(scancodeToASCII) && scancodeToASCII[key] != 0) {
                // Caps lock only affects letters, shift affects everything
                bool upper = capslock ^ shift;
                char c;
                if(shift)
                    c = scancodeToASCIIShifted[key];
                else
                    c = scancodeToASCII[key];

                if(capslock && !shift && c >= 'a' && c <= 'z')
                    c -= 32;
                else if(capslock && shift && c >= 'A' && c <= 'Z')
                    c += 32;

                char str[2] = { c, '\0' };
                printf(str);
                
            }
            break;
    }

    return esp;
}