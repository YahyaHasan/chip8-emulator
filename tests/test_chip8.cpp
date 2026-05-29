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

// ---- 2NNN / 00EE: call and return ----

TEST(Chip8Opcode, OP_2NNN_CallPushesReturnAddress) {
    Chip8 cpu;
    uint16_t pc_before = cpu.get_pc();       // 0x200
    cpu.execute(0x2400);                     // call 0x400
    EXPECT_EQ(cpu.get_pc(), 0x400);
    EXPECT_EQ(cpu.get_sp(), 1);
    // implicitly: stack[0] should hold pc_before
    // we can't read stack directly without exposing it; the return test verifies it
    (void)pc_before;
}

TEST(Chip8Opcode, OP_00EE_ReturnPopsStack) {
    Chip8 cpu;
    cpu.execute(0x2400);                     // call 0x400, pushes 0x200
    cpu.execute(0x00EE);                     // return, pops 0x200
    EXPECT_EQ(cpu.get_pc(), 0x200);
    EXPECT_EQ(cpu.get_sp(), 0);
}

TEST(Chip8Opcode, NestedCallsAndReturns) {
    Chip8 cpu;
    cpu.execute(0x2400);                     // call 0x400 from 0x200
    cpu.execute(0x2600);                     // call 0x600 from 0x400
    EXPECT_EQ(cpu.get_pc(), 0x600);
    EXPECT_EQ(cpu.get_sp(), 2);

    cpu.execute(0x00EE);                     // return to 0x400
    EXPECT_EQ(cpu.get_pc(), 0x400);
    EXPECT_EQ(cpu.get_sp(), 1);

    cpu.execute(0x00EE);                     // return to 0x200
    EXPECT_EQ(cpu.get_pc(), 0x200);
    EXPECT_EQ(cpu.get_sp(), 0);
}

// ---- 8XY0: register copy ----

TEST(Chip8Opcode, OP_8XY0_CopiesRegister) {
    Chip8 cpu;
    cpu.execute(0x6B42);                  // V_B = 0x42
    cpu.execute(0x8AB0);                  // V_A = V_B
    EXPECT_EQ(cpu.get_register(0xA), 0x42);
    EXPECT_EQ(cpu.get_register(0xB), 0x42);  // source unchanged
}

// ---- 8XY1: bitwise OR ----

TEST(Chip8Opcode, OP_8XY1_BitwiseOr) {
    Chip8 cpu;
    cpu.execute(0x6A0F);                  // V_A = 0x0F
    cpu.execute(0x6BF0);                  // V_B = 0xF0
    cpu.execute(0x8AB1);                  // V_A = V_A | V_B
    EXPECT_EQ(cpu.get_register(0xA), 0xFF);
}

// ---- 8XY2: bitwise AND ----

TEST(Chip8Opcode, OP_8XY2_BitwiseAnd) {
    Chip8 cpu;
    cpu.execute(0x6AFF);                  // V_A = 0xFF
    cpu.execute(0x6B0F);                  // V_B = 0x0F
    cpu.execute(0x8AB2);                  // V_A = V_A & V_B
    EXPECT_EQ(cpu.get_register(0xA), 0x0F);
}

// ---- 8XY3: bitwise XOR ----

TEST(Chip8Opcode, OP_8XY3_BitwiseXor) {
    Chip8 cpu;
    cpu.execute(0x6AAA);                  // V_A = 0xAA
    cpu.execute(0x6BFF);                  // V_B = 0xFF
    cpu.execute(0x8AB3);                  // V_A = V_A ^ V_B
    EXPECT_EQ(cpu.get_register(0xA), 0x55);
}

// ---- 8XY5: subtraction with borrow flag ----

TEST(Chip8Opcode, OP_8XY5_NoBorrow) {
    Chip8 cpu;
    cpu.execute(0x6A05);                  // V_A = 5
    cpu.execute(0x6B03);                  // V_B = 3
    cpu.execute(0x8AB5);                  // V_A = V_A - V_B
    EXPECT_EQ(cpu.get_register(0xA), 2);
    EXPECT_EQ(cpu.get_register(0xF), 1);  // VF = 1 means NO borrow
}

TEST(Chip8Opcode, OP_8XY5_WithBorrow) {
    Chip8 cpu;
    cpu.execute(0x6A03);                  // V_A = 3
    cpu.execute(0x6B05);                  // V_B = 5
    cpu.execute(0x8AB5);                  // V_A = V_A - V_B
    EXPECT_EQ(cpu.get_register(0xA), 0xFE);  // 3 - 5 wraps to 0xFE
    EXPECT_EQ(cpu.get_register(0xF), 0);  // VF = 0 means borrow occurred
}

TEST(Chip8Opcode, OP_8XY5_EqualValues) {
    Chip8 cpu;
    cpu.execute(0x6A05);                  // V_A = 5
    cpu.execute(0x6B05);                  // V_B = 5
    cpu.execute(0x8AB5);                  // V_A = V_A - V_B
    EXPECT_EQ(cpu.get_register(0xA), 0);
    EXPECT_EQ(cpu.get_register(0xF), 1);  // V_A >= V_B holds when equal
}

// ---- 4XNN: skip if Vx != NN ----

TEST(Chip8Opcode, OP_4XNN_SkipsWhenNotEqual) {
    Chip8 cpu;
    cpu.execute(0x6A42);                  // V_A = 0x42
    uint16_t pc_before = cpu.get_pc();
    cpu.execute(0x4A99);                  // skip if V_A != 0x99 (true)
    EXPECT_EQ(cpu.get_pc(), pc_before + 2);
}

TEST(Chip8Opcode, OP_4XNN_DoesNotSkipWhenEqual) {
    Chip8 cpu;
    cpu.execute(0x6A42);                  // V_A = 0x42
    uint16_t pc_before = cpu.get_pc();
    cpu.execute(0x4A42);                  // skip if V_A != 0x42 (false)
    EXPECT_EQ(cpu.get_pc(), pc_before);
}

// ---- 5XY0: skip if Vx == Vy ----

TEST(Chip8Opcode, OP_5XY0_SkipsWhenEqual) {
    Chip8 cpu;
    cpu.execute(0x6A42);                  // V_A = 0x42
    cpu.execute(0x6B42);                  // V_B = 0x42
    uint16_t pc_before = cpu.get_pc();
    cpu.execute(0x5AB0);                  // skip if V_A == V_B (true)
    EXPECT_EQ(cpu.get_pc(), pc_before + 2);
}

TEST(Chip8Opcode, OP_5XY0_DoesNotSkipWhenNotEqual) {
    Chip8 cpu;
    cpu.execute(0x6A42);                  // V_A = 0x42
    cpu.execute(0x6B99);                  // V_B = 0x99
    uint16_t pc_before = cpu.get_pc();
    cpu.execute(0x5AB0);                  // skip if V_A == V_B (false)
    EXPECT_EQ(cpu.get_pc(), pc_before);
}

// ---- 9XY0: skip if Vx != Vy ----

TEST(Chip8Opcode, OP_9XY0_SkipsWhenNotEqual) {
    Chip8 cpu;
    cpu.execute(0x6A42);                  // V_A = 0x42
    cpu.execute(0x6B99);                  // V_B = 0x99
    uint16_t pc_before = cpu.get_pc();
    cpu.execute(0x9AB0);                  // skip if V_A != V_B (true)
    EXPECT_EQ(cpu.get_pc(), pc_before + 2);
}

TEST(Chip8Opcode, OP_9XY0_DoesNotSkipWhenEqual) {
    Chip8 cpu;
    cpu.execute(0x6A42);                  // V_A = 0x42
    cpu.execute(0x6B42);                  // V_B = 0x42
    uint16_t pc_before = cpu.get_pc();
    cpu.execute(0x9AB0);                  // skip if V_A != V_B (false)
    EXPECT_EQ(cpu.get_pc(), pc_before);
}

// ---- ANNN: set I = NNN ----

TEST(Chip8Opcode, OP_ANNN_SetsIRegister) {
    Chip8 cpu;
    cpu.execute(0xA123);                  // I = 0x123
    EXPECT_EQ(cpu.get_I(), 0x123);
}

TEST(Chip8Opcode, OP_ANNN_DoesNotAffectPC) {
    Chip8 cpu;
    uint16_t pc_before = cpu.get_pc();
    cpu.execute(0xA123);
    EXPECT_EQ(cpu.get_pc(), pc_before);   // ANNN must not touch PC
}

