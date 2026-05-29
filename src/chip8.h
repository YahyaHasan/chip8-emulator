#pragma once

#include <array>
#include <cstdint>

class Chip8 {
public:
    Chip8();
    
    void cycle();
    void execute(uint16_t opcode);

    uint8_t get_register(uint8_t index) const;
    uint16_t get_pc() const {return pc; }
    uint8_t get_sp() const {return sp; }
    uint16_t get_I() const {return I; }

private:
    uint16_t fetch();
    std::array<uint8_t, 16>   V{};
    std::array<uint8_t, 4096> memory{};
    std:: array<uint16_t, 16> stack{};
    uint16_t pc = 0x200;
    uint16_t I = 0;
    uint8_t sp = 0;
};