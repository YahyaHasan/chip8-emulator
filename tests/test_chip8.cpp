#include <gtest/gtest.h>
#include "chip8.h"

TEST(Chip8Opcode, OP_6XNN_LoadImmediate) {
    Chip8 cpu;
    cpu.execute(0x6A42);                  // V_A = 0x42
    EXPECT_EQ(cpu.get_register(0xA), 0x42);
}

TEST(Chip8Opcode, OP_6XNN_DoesNotAffectOtherRegisters) {
    Chip8 cpu;
    cpu.execute(0x6A42);
    EXPECT_EQ(cpu.get_register(0x0), 0x00);
    EXPECT_EQ(cpu.get_register(0xB), 0x00);
    EXPECT_EQ(cpu.get_register(0xF), 0x00);
}

// ---- 7XNN: add immediate ----

TEST(Chip8Opcode, OP_7XNN_AddImmediate) {
    Chip8 cpu;
    cpu.execute(0x6A05);   // V_A = 5
    cpu.execute(0x7A03);   // V_A += 3
    EXPECT_EQ(cpu.get_register(0xA), 8);
}

TEST(Chip8Opcode, OP_7XNN_WrapsWithoutSettingVF) {
    Chip8 cpu;
    cpu.execute(0x6AFF);   // V_A = 0xFF
    cpu.execute(0x7A01);   // V_A += 1, wraps to 0
    EXPECT_EQ(cpu.get_register(0xA), 0);
    EXPECT_EQ(cpu.get_register(0xF), 0);  // 7XNN must NOT set VF
}

// ---- 1NNN: jump ----

TEST(Chip8Opcode, OP_1NNN_SetsPC) {
    Chip8 cpu;
    cpu.execute(0x1ABC);
    EXPECT_EQ(cpu.get_pc(), 0xABC);
}

// ---- 3XNN: skip if Vx == NN ----

TEST(Chip8Opcode, OP_3XNN_SkipsWhenEqual) {
    Chip8 cpu;
    cpu.execute(0x6A42);                       // V_A = 0x42
    uint16_t pc_before = cpu.get_pc();
    cpu.execute(0x3A42);                       // skip if V_A == 0x42 (yes)
    EXPECT_EQ(cpu.get_pc(), pc_before + 2);
}

TEST(Chip8Opcode, OP_3XNN_DoesNotSkipWhenNotEqual) {
    Chip8 cpu;
    cpu.execute(0x6A42);                       // V_A = 0x42
    uint16_t pc_before = cpu.get_pc();
    cpu.execute(0x3A99);                       // skip if V_A == 0x99 (no)
    EXPECT_EQ(cpu.get_pc(), pc_before);
}

// ---- 8XY4: register-register add with carry ----

TEST(Chip8Opcode, OP_8XY4_AddNoCarry) {
    Chip8 cpu;
    cpu.execute(0x6A05);                       // V_A = 5
    cpu.execute(0x6B03);                       // V_B = 3
    cpu.execute(0x8AB4);                       // V_A = V_A + V_B
    EXPECT_EQ(cpu.get_register(0xA), 8);
    EXPECT_EQ(cpu.get_register(0xF), 0);       // no carry
}

TEST(Chip8Opcode, OP_8XY4_WithCarry) {
    Chip8 cpu;
    cpu.execute(0x6AFF);                       // V_A = 0xFF
    cpu.execute(0x6B01);                       // V_B = 0x01
    cpu.execute(0x8AB4);                       // 0xFF + 0x01 = 0x100, wraps
    EXPECT_EQ(cpu.get_register(0xA), 0x00);
    EXPECT_EQ(cpu.get_register(0xF), 1);       // carry set
}