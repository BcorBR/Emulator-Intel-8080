# Intel 8080 Emulator 🚀

This project is an **Intel 8080 processor emulator**, with the ultimate goal of running the classic game **Space Invaders** 👾. The Intel 8080 was an 8-bit processor released in 1974, widely used in embedded systems and arcades, such as in Space Invaders.

## 🎯 Objective

To develop an emulator that simulates the behavior of the Intel 8080 processor, allowing the execution of the Space Invaders game on modern hardware. The project focuses on fidelity to the original architecture, ensuring that all instructions and system behavior are correctly emulated.

## 🔧 Features

- Complete emulation of the Intel 8080 processor instruction set.
- Implementation of a memory system for reading and writing.
- Support for interrupts and peripherals from the original hardware.
- Ability to load and execute the binary code of the Space Invaders game.
- Rendering screen and basic support for graphics and sound.

## 🏗️ How It Works

The emulator reads the binary code of the Space Invaders game and simulates the execution of each instruction of the 8080 processor. It also simulates the interrupts and peripherals needed for the proper execution of the game, such as the screen and input control.

### ⚙️ Intel 8080 Architecture

The Intel 8080 is an 8-bit processor with 7 eight-bit registers, a stack pointer, and a program counter. This emulator implements:

- **Instruction set**: All 256 instructions of the 8080, including data movement, logical and arithmetic operations, jumps, calls, and returns.
- **Memory system**: Simulation of RAM to support the game and code space.
- **Interrupts**: Implementation of the necessary interrupts for the Space Invaders game.

