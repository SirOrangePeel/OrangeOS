# OrangeOS
 
> A hobby operating system built from scratch.
 
## About
 
OrangOS is a bare-metal operating system written in C++/Assembly. This project is a work in progress — currently in the earliest stages of development.
 
## Current Features
- [x] Hello World Kernel
- [x] Memory Segments & Global Descriptor Table (GDT)
- [x] Hardware Communication & Ports
- [x] Interrupts & IRQ handling (IDT, PIC remapping)
- [x] Keyboard driver
- [x] Mouse driver
- [ ] Driver Abstraction
 
## Getting Started
 
### Prerequisites
 
- GCC, NASM
- Virtual Box
- Make
 
### Building
 
```bash
git clone https://github.com/SirOrangePeel/OrangeOS.git
cd OrangeOS
make
```
 
### Running
 
```bash
make run
# or manually:
qemu-system-i386 -kernel myos.bin
```
 
## Project Structure
 
```
OrangeOS/
├── includes/
│   ├── gdt.h           # Global Descriptor Table declarations
│   ├── interrupts.h    # Interrupt handling declarations
|   ├── keyboard.h      # Keyboard function definitions
│   ├── port.h          # Port I/O declarations
│   └── types.h         # Common type definitions
├── src/
│   ├── bootloader/
│   │   └── loader.s    # Assembly bootloader entry point
│   ├── cpu/
│   │   ├── gdt.cpp         # Global Descriptor Table implementation
│   │   ├── interrupts.cpp  # Interrupt handling implementation
│   │   ├── interruptstubs.s # Low-level interrupt stubs in assembly
│   │   └── port.cpp        # Port I/O implementation
|   ├── drivers/
|   |   └── keyboardDriver.cpp # Keyboard Driver implementation
│   └── kernel/
│       └── kernel.cpp  # Kernel entry point
├── .gitignore
├── LICENSE
├── linker.ld           # Linker script
├── Makefile
├── README.md
└── toggle_kvm_for_vbox.sh  # Helper script to toggle KVM for VirtualBox
```

## Resources & Inspiration
 
- [OSDev Wiki](https://wiki.osdev.org/)
- [Operating Systems: Three Easy Pieces](https://ia600600.us.archive.org/7/items/operating-systems-three-easy-pieces/Remzi%20H.%20Arpaci-Dusseau%2C%20Andrea%20C.%20Arpaci-Dusseau%20-%20Operating%20Systems%20-%20Three%20Easy%20Pieces.pdf)
- [GeeksForGeeks: Operating Systems](https://www.geeksforgeeks.org/operating-systems/operating-systems/)
- [Osdev Notes](https://github.com/dreamportdev/Osdev-Notes/releases/tag/latest-master)
- [Write your own Operating System](https://www.youtube.com/playlist?list=PLHh55M_Kq4OApWScZyPl5HhgsTJS9MZ6M)


## License
 
[MIT](LICENSE)
 
