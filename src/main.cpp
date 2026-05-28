#include "chip8.h"
#include <iostream>

int main() {
    Chip8 cpu;

    // test a simple instruction: V0 = 0xAA
    cpu.execute(0x60AA);

    std::cout << "V0 = 0x"
              << std::hex
              << (int)cpu.get_register(0)
              << std::dec
              << "\n";

    return 0;
}