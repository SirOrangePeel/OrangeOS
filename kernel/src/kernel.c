#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "Needs to be compiled with a ix86-elf compiler"
#endif

/* Hardware text mode colour constants. */
enum vgaColour {
	VGA_COLOUR_BLACK = 0,
	VGA_COLOUR_BLUE = 1,
	VGA_COLOUR_GREEN = 2,
	VGA_COLOUR_CYAN = 3,
	VGA_COLOUR_RED = 4,
	VGA_COLOUR_MAGENTA = 5,
	VGA_COLOUR_BROWN = 6,
	VGA_COLOUR_LIGHT_GREY = 7,
	VGA_COLOUR_DARK_GREY = 8,
	VGA_COLOUR_LIGHT_BLUE = 9,
	VGA_COLOUR_LIGHT_GREEN = 10,
	VGA_COLOUR_LIGHT_CYAN = 11,
	VGA_COLOUR_LIGHT_RED = 12,
	VGA_COLOUR_LIGHT_MAGENTA = 13,
	VGA_COLOUR_LIGHT_BROWN = 14,
	VGA_COLOUR_WHITE = 15,
};

static inline uint8_t VgaEntryColour(enum vgaColour fg, enum vgaColour bg) 
{
	return fg | bg << 4;
}

static inline uint16_t VgaEntry(unsigned char uc, uint8_t colour) 
{
	return (uint16_t) uc | (uint16_t) colour << 8;
}

size_t kstrlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  0xB8000 

size_t terminalRow;
size_t terminalColumn;
uint8_t terminalColour;
uint16_t* terminalBuffer = (uint16_t*)VGA_MEMORY;

void Terminal_Initialize(void) 
{
	terminalRow = 0;
	terminalColumn = 0;
	terminalColour = VgaEntryColour(VGA_COLOUR_LIGHT_GREY, VGA_COLOUR_BLACK);
	
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminalBuffer[index] = VgaEntry(' ', terminalColour);
		}
	}
}

void Terminal_SetColour(uint8_t colour) 
{
	terminalColour = colour;
}

void Terminal_PutEntryAt(char c, uint8_t colour, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminalBuffer[index] = VgaEntry(c, colour);
}

void Terminal_PutChar(char c) 
{
    if(c == '\n') {
        terminalColumn = 0;
        if (++terminalRow == VGA_HEIGHT)
			terminalRow = 0;
        return;
    }

	Terminal_PutEntryAt(c, terminalColour, terminalColumn, terminalRow);
	if (++terminalColumn == VGA_WIDTH) {
		terminalColumn = 0;
		if (++terminalRow == VGA_HEIGHT)
			terminalRow = 0;
	}
}

void Terminal_Write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		Terminal_PutChar(data[i]);
}

void Terminal_WriteString(const char* data) 
{
	Terminal_Write(data, kstrlen(data));
}

void Kernel_Main(void) 
{
	/* Initialize terminal interface */
	Terminal_Initialize();

	Terminal_WriteString("Hello, kernel World!\n");
}