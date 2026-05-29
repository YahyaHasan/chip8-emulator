#include "chip8.h"

Chip8::Chip8() {
    // setup beyond the in-class initializers, if any
}

void Chip8::execute(uint16_t opcode) {
    uint8_t hi_nibble = (opcode & 0xF000) >> 12;
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t n = opcode & 0x000F;
    uint8_t nn =  opcode & 0x00FF;
    uint16_t nnn = opcode & 0x0FFF;

    switch (hi_nibble) {
        case 0x0:
            if (opcode == 0x00EE) {
                sp--;
                pc = stack[sp];
            }
            break;

        case 0x1:
            pc = nnn;
            break;
        
        case 0x2:
            stack[sp] = pc;
            sp++;
            pc = nnn;
            break;

        case 0x3:
            if (V[x] == nn) {
                pc += 2;
            }
            break;

        case 0x6:
            V[x] = nn;
            break;
        
        case 0x7:
            V[x] += nn;
            break;
        
        case 0x8:
            switch(n) {
                case 0x4:
                    uint16_t sum = static_cast<uint16_t>(V[x]) + static_cast<uint16_t>(V[y]);
                    V[x] = static_cast<uint8_t>(sum & 0xFF);
                    V[0xF] = (sum > 0xFF) ? 1 : 0;
                    break;
            }
            break;
    }
}

uint8_t Chip8::get_register(uint8_t index) const {
    return V[index];
}

void Chip8::cycle() {
    uint16_t opcode = fetch();
    execute(opcode);
}

uint16_t Chip8::fetch() {
    uint16_t opcode = (memory[pc] << 8) | (memory[pc + 1]);
    pc += 2;
    return opcode;
}