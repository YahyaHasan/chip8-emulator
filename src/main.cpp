#include "chip8.h"
#include <iostream>
#include <string>

// std::cout treats uint8_t as a character. Cast to int to print the number.
static int as_int(uint8_t v) { return static_cast<int>(v); }

// Print a section header.
static void section(const std::string& title) {
    std::cout << "\n=== " << title << " ===\n";
}

int main() {
    std::cout << "CHIP-8 Emulator Demo\n";
    std::cout << "====================\n";

    // --- Immediate load and add ---
    section("Immediate load (6XNN) and add (7XNN)");
    {
        Chip8 cpu;
        cpu.execute(0x6005);              // V0 = 5
        cpu.execute(0x7003);              // V0 += 3
        std::cout << "  V0 = 5, then += 3  ->  V0 = "
                  << as_int(cpu.get_register(0)) << "\n";
    }

    // --- Register-register add with carry ---
    section("Register-register add with carry (8XY4)");
    {
        Chip8 cpu;
        cpu.execute(0x60FF);              // V0 = 0xFF
        cpu.execute(0x6101);              // V1 = 0x01
        cpu.execute(0x8014);              // V0 = V0 + V1
        std::cout << "  0xFF + 0x01  ->  V0 = 0x" << std::hex
                  << as_int(cpu.get_register(0))
                  << ", VF = " << as_int(cpu.get_register(0xF))
                  << std::dec << " (carry set)\n";
    }

    // --- Register copy and bitwise logic ---
    section("Register copy and logical ops (8XY0-8XY3)");
    {
        Chip8 cpu;
        cpu.execute(0x6142);              // V1 = 0x42
        cpu.execute(0x8010);              // V0 = V1 (copy)
        std::cout << "  copy V1 (0x42) -> V0  =  0x" << std::hex
                  << as_int(cpu.get_register(0)) << std::dec << "\n";

        Chip8 c2;
        c2.execute(0x600F); c2.execute(0x61F0); c2.execute(0x8011);
        std::cout << "  0x0F | 0xF0  =  0x" << std::hex
                  << as_int(c2.get_register(0)) << std::dec << "\n";

        Chip8 c3;
        c3.execute(0x60FF); c3.execute(0x610F); c3.execute(0x8012);
        std::cout << "  0xFF & 0x0F  =  0x" << std::hex
                  << as_int(c3.get_register(0)) << std::dec << "\n";

        Chip8 c4;
        c4.execute(0x60AA); c4.execute(0x61FF); c4.execute(0x8013);
        std::cout << "  0xAA ^ 0xFF  =  0x" << std::hex
                  << as_int(c4.get_register(0)) << std::dec << "\n";
    }

    // --- Subtract with borrow ---
    section("Subtract with borrow flag (8XY5)");
    {
        Chip8 cpu;
        cpu.execute(0x6005); cpu.execute(0x6103); cpu.execute(0x8015);
        std::cout << "  5 - 3  ->  V0 = " << as_int(cpu.get_register(0))
                  << ", VF = " << as_int(cpu.get_register(0xF)) << " (no borrow)\n";

        Chip8 c2;
        c2.execute(0x6003); c2.execute(0x6105); c2.execute(0x8015);
        std::cout << "  3 - 5  ->  V0 = 0x" << std::hex
                  << as_int(c2.get_register(0)) << std::dec
                  << ", VF = " << as_int(c2.get_register(0xF))
                  << " (borrow occurred, result wrapped)\n";
    }

    // --- Unconditional jump ---
    section("Unconditional jump (1NNN)");
    {
        Chip8 cpu;
        cpu.execute(0x1ABC);
        std::cout << "  jump 0xABC  ->  pc = 0x" << std::hex
                  << cpu.get_pc() << std::dec << "\n";
    }

    // --- Conditional skips ---
    section("Conditional skips (3XNN, 4XNN, 5XY0, 9XY0)");
    {
        auto skip_amount = [](uint16_t before, uint16_t after) {
            return after - before;
        };

        Chip8 c1; c1.execute(0x6042);
        uint16_t b1 = c1.get_pc(); c1.execute(0x3042);
        std::cout << "  3XNN: V0 == 0x42  ->  pc advanced by "
                  << skip_amount(b1, c1.get_pc()) << "\n";

        Chip8 c2; c2.execute(0x6042);
        uint16_t b2 = c2.get_pc(); c2.execute(0x4042);
        std::cout << "  4XNN: V0 != 0x42 (false)  ->  pc advanced by "
                  << skip_amount(b2, c2.get_pc()) << " (no skip)\n";

        Chip8 c3; c3.execute(0x6042); c3.execute(0x6142);
        uint16_t b3 = c3.get_pc(); c3.execute(0x5010);
        std::cout << "  5XY0: V0 == V1  ->  pc advanced by "
                  << skip_amount(b3, c3.get_pc()) << "\n";

        Chip8 c4; c4.execute(0x6042); c4.execute(0x6199);
        uint16_t b4 = c4.get_pc(); c4.execute(0x9010);
        std::cout << "  9XY0: V0 != V1  ->  pc advanced by "
                  << skip_amount(b4, c4.get_pc()) << "\n";
    }

    // --- Subroutine call and return ---
    section("Subroutine call and return (2NNN, 00EE)");
    {
        Chip8 cpu;
        std::cout << "  initial            pc = 0x" << std::hex
                  << cpu.get_pc() << ", sp = " << std::dec
                  << as_int(cpu.get_sp()) << "\n";
        cpu.execute(0x2400);
        std::cout << "  call 0x400         pc = 0x" << std::hex
                  << cpu.get_pc() << ", sp = " << std::dec
                  << as_int(cpu.get_sp()) << "\n";
        cpu.execute(0x2600);
        std::cout << "  nested call 0x600  pc = 0x" << std::hex
                  << cpu.get_pc() << ", sp = " << std::dec
                  << as_int(cpu.get_sp()) << "\n";
        cpu.execute(0x00EE);
        std::cout << "  return             pc = 0x" << std::hex
                  << cpu.get_pc() << ", sp = " << std::dec
                  << as_int(cpu.get_sp()) << "\n";
        cpu.execute(0x00EE);
        std::cout << "  return             pc = 0x" << std::hex
                  << cpu.get_pc() << ", sp = " << std::dec
                  << as_int(cpu.get_sp()) << "\n";
    }

    // --- Index register ---
    section("Index register (ANNN)");
    {
        Chip8 cpu;
        cpu.execute(0xA300);
        std::cout << "  ANNN  ->  I = 0x" << std::hex
                  << cpu.get_I() << std::dec << "\n";
    }

    std::cout << "\nDemo complete.\n";
    return 0;
}