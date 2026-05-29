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
            if (opcode == 0x00EE) { // 00EE: return from subroutine
                sp--;
                pc = stack[sp];
            }
            break;

        case 0x1:                   // 1NNN: jump
            pc = nnn;
            break;
        
        case 0x2:                   // 2NNN: call subroutine at NNN
            stack[sp] = pc;
            sp++;
            pc = nnn;
            break;

        case 0x3:                   // 3XNN: skip next if Vx == NN
            if (V[x] == nn) {
                pc += 2;
            }
            break;
        
        case 0x4:                   // 4XNN: skip if Vx != NN 
            if (V[x] != nn) {
                pc += 2;
            }
            break;
        
        case 0x5:                   // 5XY0: skip if Vx == Vy
            if (V[x] == V[y]) {
                pc +=2;
            }
            break;

        case 0x6:                   // 6XNN: Vx = NN
            V[x] = nn;
            break;
        
        case 0x7:                   // 7XNN: Vx += NN, no flag update
            V[x] += nn;
            break;
        
        case 0x8:                   // 8XYN: register-register ops, sub-dispatch on n
            switch(n) {
                case 0x4: {         // 8XY4: Vx = Vx + Vy, VF = carry
                    uint16_t sum = static_cast<uint16_t>(V[x]) + static_cast<uint16_t>(V[y]);
                    V[x] = static_cast<uint8_t>(sum & 0xFF);
                    V[0xF] = (sum > 0xFF) ? 1 : 0;
                    break;
                }
                
                case 0x0:                   // 8XY0: Vx = Vy
                    V[x] = V[y];
                    break;
                
                case 0x1:                   // 8XY1: Vx = Vx | Vy
                    V[x] = V[x] | V[y];
                    break;
                
                case 0x2:                   // 8XY2: Vx = Vx & Vy
                    V[x] = V[x] & V[y];
                    break;

                case 0x3:                   // 8XY3: Vx = Vx ^ Vy
                    V[x] = V[x] ^ V[y];
                    break;

                case 0x5: {                 // 8XY5: Vx = Vx - Vy, VF = NOT borrow
                    uint8_t diff = V[x] - V[y];
                    bool no_borrow = V[x] >= V[y];
                    V[x] = diff;
                    V[0xF] = no_borrow ? 1 : 0;
                    break;
                }
            }
            break;
        
        case 0x9:                   // 9XY0: skip if Vx != Vy
            if (V[x] != V[y]) {
                pc += 2;
            }
            break;
        
        case 0xA:                   // ANNN: I = NNN
            I = nnn;
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