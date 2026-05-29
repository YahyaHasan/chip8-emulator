#include "chip8.h"
#include <iostream>

int main() {
    Chip8 cpu;

    std::cout << "CHIP-8 Emulator Demo\n";
    std::cout << "====================\n\n";

    // Hand-execute: V0 = 10, V1 = 5, V0 = V0 + V1
    cpu.execute(0x60FF);
    cpu.execute(0x6105);
    cpu.execute(0x8014);

    std::cout << "After 255 + 5:\n";
    std::cout << "  V0 = " << static_cast<int>(cpu.get_register(0)) << "\n";
    std::cout << "  VF = " << static_cast<int>(cpu.get_register(0xF)) << "(carry)\n";


    return 0;
}