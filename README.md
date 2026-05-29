# CHIP-8 Emulator

A minimal CHIP-8 interpreter in C++17. Built as scaffolding for a RISC-V RV32I emulator, focused on the fetch-decode-execute architecture pattern rather than full CHIP-8 spec compliance.

## Implemented

- Fetch-decode-execute cycle with big-endian opcode assembly
- 14 representative opcodes covering the core ISA patterns:
  - Immediate load and add: `6XNN`, `7XNN`
  - Control flow: `1NNN` (jump), `2NNN`/`00EE` (call/return)
  - Conditional skips: `3XNN`, `4XNN`, `5XY0`, `9XY0`
  - Register-register ALU: `8XY0` through `8XY5`
  - Index register: `ANNN`
- 16-level subroutine call stack
- Unit tests covering both branches of each conditional and the edge cases for arithmetic with carry/borrow flags

## Intentionally skipped

- Display rendering (no SDL or framebuffer)
- Keyboard input (stubbed as no key pressed)
- Sound and 60Hz timers
- Shift/alternate-subtract opcodes (`8XY6`, `8XY7`, `8XYE`)
- Display, input, and sound opcodes (`DXYN`, `EX9E`/`EXA1`, `FXNN` family)

These omissions are deliberate. The goal was to internalize the emulator architecture pattern before moving to a RISC-V RV32I emulator, where the same patterns apply with wider operands and more instruction formats.

## Build and run

```bash
mkdir build && cd build
cmake ..
cmake --build .
./chip8                      # runs the demo
ctest --output-on-failure    # runs the test suite
```

## Tooling

- C++17, CMake 3.14+
- GoogleTest, fetched automatically by CMake