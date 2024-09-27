#include <stdlib.h>
#include <stdint.h>

typedef struct ConditionCodes{
    uint8_t    z:1;
    uint8_t    s:1;
    uint8_t    p:1;
    uint8_t    cy:1;
    uint8_t    ac:1;
    uint8_t    pad:3; 
} ConditionCodes;

typedef struct State8080{
    uint8_t    a;
    uint8_t    b;
    uint8_t    c;
    uint8_t    d;
    uint8_t    e;
    uint8_t    h;
    uint8_t    l;
    uint16_t    sp;
    uint16_t    pc;
    uint8_t    *memory;
    struct    ConditionCodes    cc;
    uint8_t    int_enable;
} State8080;

void UnimplementedInstruction(State8080 *state){
    state->pc--;
    printf("Error: instruction not implemented\n");
    exit(EXIT_FAILURE);
}

int Emulate8080Op(State8080 *state){
    unsigned char *opcode = &state->memory[state->pc];

    switch(*opcode){
        case 0x00: 
            break;
        
        // Description: The third byte of the instruction (the \
           most significant 8 bits of the 16-bit immediate data) is \
           loaded into the first register of the specified pair, while the \
           second byte of the instruction (the least significant 8 bits of \
           the 16-bit immediate data) is loaded into the second register \
           of the specified pair. If SP is specified as the register pair, the \
           second byte of the instruction replaces the least significant \
           8 bits of the stack pointer, while the third byte of the in- \
           struction replaces the most significant 8 bits of the stack \
           pointer.
        // LXI BC
        case 0x01: 
            state->b = opcode[2];
            state->c = opcode[1]; 
            state->pc += 2;
            break;
        
        // Description: The contents of the accumulator are \
           stored in the memory location addressed by registers B \
           and C, or by registers D and E.
        case 0x02:
            state->memory[(state->b << 8) | state->c] = state->a; 
            break;
        
        // INX Increment Register Pair \
           Description: The 16-bit number held in the specified \
           register pair is incremented by one.
        case 0x03:
            uint16_t temp = (state->b << 8) | state->c;
            temp++;
            state->b = temp >> 8;
            state->c = temp & 0b11111111;
            break;
        case 0x04: UnimplementedInstruction(state); break;
        case 0x05: UnimplementedInstruction(state); break;
        
        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI B
        case 0x06: 
            state->b = opcode[1];
            state->pc += 1;
            break;
        case 0x07: UnimplementedInstruction(state); break;
        case 0x08: UnimplementedInstruction(state); break;
        case 0x09: UnimplementedInstruction(state); break;
        case 0x0a: UnimplementedInstruction(state); break;
        case 0x0b: UnimplementedInstruction(state); break;
        case 0x0c: UnimplementedInstruction(state); break;
        case 0x0d: UnimplementedInstruction(state); break;

        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI C
        case 0x0e: 
            state->c = opcode[1];
            state->pc += 1;
            break;
        case 0x0f: UnimplementedInstruction(state); break;
        case 0x10: UnimplementedInstruction(state); break;
        
        // Description: The third byte of the instruction (the \
           most significant 8 bits of the 16-bit immediate data) is \
           loaded into the first register of the specified pair, while the \
           second byte of the instruction (the least significant 8 bits of \
           the 16-bit immediate data) is loaded into the second register \
           of the specified pair. If SP is specified as the register pair, the \
           second byte of the instruction replaces the least significant \
           8 bits of the stack pointer, while the third byte of the in- \
           struction replaces the most significant 8 bits of the stack \
           pointer.
        // LXI DE
        case 0x11: 
            state->d = opcode[2];
            state->e = opcode[1]; 
            state->pc += 2;
            break;
        case 0x12: UnimplementedInstruction(state); break;
        case 0x13: UnimplementedInstruction(state); break;
        case 0x14: UnimplementedInstruction(state); break;
        case 0x15: UnimplementedInstruction(state); break;

        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI D
        case 0x16: 
            state->d = opcode[1];
            state->pc += 1;
            break;
        case 0x17: UnimplementedInstruction(state); break;
        case 0x18: UnimplementedInstruction(state); break;
        case 0x19: UnimplementedInstruction(state); break;
        case 0x1a: UnimplementedInstruction(state); break;
        case 0x1b: UnimplementedInstruction(state); break;
        case 0x1c: UnimplementedInstruction(state); break;
        case 0x1d: UnimplementedInstruction(state); break;

        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI E
        case 0x1e: 
            state->e = opcode[1];
            state->pc += 1;
            break;
        case 0x1f: UnimplementedInstruction(state); break;
        case 0x20: UnimplementedInstruction(state); break;

        // Description: The third byte of the instruction (the \
           most significant 8 bits of the 16-bit immediate data) is \
           loaded into the first register of the specified pair, while the \
           second byte of the instruction (the least significant 8 bits of \
           the 16-bit immediate data) is loaded into the second register \
           of the specified pair. If SP is specified as the register pair, the \
           second byte of the instruction replaces the least significant \
           8 bits of the stack pointer, while the third byte of the in- \
           struction replaces the most significant 8 bits of the stack \
           pointer.
        // LXI HL
        case 0x21: 
            state->h = opcode[2];
            state->l = opcode[1]; 
            state->pc += 2;
            break;
        case 0x22: UnimplementedInstruction(state); break;
        case 0x23: UnimplementedInstruction(state); break;
        case 0x24: UnimplementedInstruction(state); break;
        case 0x25: UnimplementedInstruction(state); break;

        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI H
        case 0x26: 
            state->h = opcode[1];
            state->pc += 1;
            break;
        case 0x27: UnimplementedInstruction(state); break;
        case 0x28: UnimplementedInstruction(state); break;
        case 0x29: UnimplementedInstruction(state); break;
        case 0x2a: UnimplementedInstruction(state); break;
        case 0x2c: UnimplementedInstruction(state); break;
        case 0x2b: UnimplementedInstruction(state); break;
        case 0x2d: UnimplementedInstruction(state); break;

        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI L
        case 0x2e: 
            state->l = opcode[1];
            state->pc += 1;
            break;
        case 0x2f: UnimplementedInstruction(state); break;
        case 0x30: UnimplementedInstruction(state); break;

        // Description: The third byte of the instruction (the \
           most significant 8 bits of the 16-bit immediate data) is \
           loaded into the first register of the specified pair, while the \
           second byte of the instruction (the least significant 8 bits of \
           the 16-bit immediate data) is loaded into the second register \
           of the specified pair. If SP is specified as the register pair, the \
           second byte of the instruction replaces the least significant \
           8 bits of the stack pointer, while the third byte of the in- \
           struction replaces the most significant 8 bits of the stack \
           pointer.
        // LXI SP
        case 0x31: 
            state->sp = (opcode[2] << 8) | opcode[1];
            state->pc += 2;            
            break;
        case 0x32: UnimplementedInstruction(state); break;
        case 0x33: UnimplementedInstruction(state); break;
        case 0x34: UnimplementedInstruction(state); break;
        case 0x35: UnimplementedInstruction(state); break;

        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI ref M (H + L)
        case 0x36: 
            state->memory[(state->h << 8) | state->l] = opcode[1];
            state->pc += 1;
            break;
        case 0x37: UnimplementedInstruction(state); break;
        case 0x38: UnimplementedInstruction(state); break;
        case 0x39: UnimplementedInstruction(state); break;
        case 0x3a: UnimplementedInstruction(state); break;
        case 0x3b: UnimplementedInstruction(state); break;
        case 0x3c: UnimplementedInstruction(state); break;
        case 0x3d: UnimplementedInstruction(state); break;

        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI A
        case 0x3e: 
            state->a = opcode[1];
            state->pc += 1;
            break;
        case 0x3f: UnimplementedInstruction(state); break;
        case 0x40: UnimplementedInstruction(state); break;
        case 0x41: UnimplementedInstruction(state); break;
        case 0x42: UnimplementedInstruction(state); break;
        case 0x43: UnimplementedInstruction(state); break;
        case 0x44: UnimplementedInstruction(state); break;
        case 0x45: UnimplementedInstruction(state); break;
        case 0x46: UnimplementedInstruction(state); break;
        case 0x47: UnimplementedInstruction(state); break;
        case 0x48: UnimplementedInstruction(state); break;
        case 0x49: UnimplementedInstruction(state); break;
        case 0x4a: UnimplementedInstruction(state); break;
        case 0x4b: UnimplementedInstruction(state); break;
        case 0x4c: UnimplementedInstruction(state); break;
        case 0x4d: UnimplementedInstruction(state); break;
        case 0x4e: UnimplementedInstruction(state); break;
        case 0x4f: UnimplementedInstruction(state); break;
        case 0x50: UnimplementedInstruction(state); break;
        case 0x51: UnimplementedInstruction(state); break;
        case 0x52: UnimplementedInstruction(state); break;
        case 0x53: UnimplementedInstruction(state); break;
        case 0x54: UnimplementedInstruction(state); break;
        case 0x55: UnimplementedInstruction(state); break;
        case 0x56: UnimplementedInstruction(state); break;
        case 0x57: UnimplementedInstruction(state); break;
        case 0x58: UnimplementedInstruction(state); break;
        case 0x59: UnimplementedInstruction(state); break;
        case 0x5a: UnimplementedInstruction(state); break;
        case 0x5b: UnimplementedInstruction(state); break;
        case 0x5c: UnimplementedInstruction(state); break;
        case 0x5d: UnimplementedInstruction(state); break;
        case 0x5e: UnimplementedInstruction(state); break;
        case 0x5f: UnimplementedInstruction(state); break;
        case 0x60: UnimplementedInstruction(state); break;
        case 0x61: UnimplementedInstruction(state); break;
        case 0x62: UnimplementedInstruction(state); break;
        case 0x63: UnimplementedInstruction(state); break;
        case 0x64: UnimplementedInstruction(state); break;
        case 0x65: UnimplementedInstruction(state); break;
        case 0x66: UnimplementedInstruction(state); break;
        case 0x67: UnimplementedInstruction(state); break;
        case 0x68: UnimplementedInstruction(state); break;
        case 0x69: UnimplementedInstruction(state); break;
        case 0x6a: UnimplementedInstruction(state); break;
        case 0x6b: UnimplementedInstruction(state); break;
        case 0x6c: UnimplementedInstruction(state); break;
        case 0x6d: UnimplementedInstruction(state); break;
        case 0x6e: UnimplementedInstruction(state); break;
        case 0x6f: UnimplementedInstruction(state); break;
        case 0x70: UnimplementedInstruction(state); break;
        case 0x71: UnimplementedInstruction(state); break;
        case 0x72: UnimplementedInstruction(state); break;
        case 0x73: UnimplementedInstruction(state); break;
        case 0x74: UnimplementedInstruction(state); break;
        case 0x75: UnimplementedInstruction(state); break;
        case 0x76: UnimplementedInstruction(state); break;
        case 0x77: UnimplementedInstruction(state); break;
        case 0x78: UnimplementedInstruction(state); break;
        case 0x79: UnimplementedInstruction(state); break;
        case 0x7a: UnimplementedInstruction(state); break;
        case 0x7b: UnimplementedInstruction(state); break;
        case 0x7c: UnimplementedInstruction(state); break;
        case 0x7d: UnimplementedInstruction(state); break;
        case 0x7e: UnimplementedInstruction(state); break;
        case 0x7f: UnimplementedInstruction(state); break;
        
        // Instructions in this class operate on the accumulator \
           using the byte in the register specified by REG. If a memory \
           reference is specified, the instructions use the byte in the \
           memory location addressed by registers Hand L. The H reg- \
           ister holds the most significant 8 bits of the address, while \
           the L register holds the least significant 8 bits of the address. 
        // Description: The specified byte is added to the con· \
           tents of the accumulator using two's complement arithmetic.
        // ADD B
        case 0x80:
            // do the math with higher precision for carry out \
               analysis
            uint16_t res = (uint16_t) state->a + (uint16_t) state->b;
            // zero flag
            if ((res & 0b11111111) == 0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (res & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            if (res > 0b11111111)
                state->cc.cy = 1;
            else
                state->cc.cy = 0;
            
            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;
            
            break;

        // Instructions in this class operate on the accumulator \
           using the byte in the register specified by REG. If a memory \
           reference is specified, the instructions use the byte in the \
           memory location addressed by registers Hand L. The H reg- \
           ister holds the most significant 8 bits of the address, while \
           the L register holds the least significant 8 bits of the address. 
        // Description: The specified byte is added to the con· \
           tents of the accumulator using two's complement arithmetic.
        // ADD C
        case 0x81: 
            // do the math with higher precision for carry out \
               analysis
            uint16_t res = (uint16_t) state->a + (uint16_t) state->c;
            // zero flag
            if ((res & 0b11111111) == 0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (res & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            if (res > 0b11111111)
                state->cc.cy = 1;
            else
                state->cc.cy = 0;
            
            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;
            
            break;

        // Instructions in this class operate on the accumulator \
           using the byte in the register specified by REG. If a memory \
           reference is specified, the instructions use the byte in the \
           memory location addressed by registers Hand L. The H reg- \
           ister holds the most significant 8 bits of the address, while \
           the L register holds the least significant 8 bits of the address. 
        // Description: The specified byte is added to the con· \
           tents of the accumulator using two's complement arithmetic.
        // ADD D
        case 0x82:
            // do the math with higher precision for carry out \
               analysis
            uint16_t res = (uint16_t) state->a + (uint16_t) state->d;
            // zero flag
            if ((res & 0b11111111) == 0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (res & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            if (res > 0b11111111)
                state->cc.cy = 1;
            else
                state->cc.cy = 0;
            
            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;
            
            break;

        // Instructions in this class operate on the accumulator \
           using the byte in the register specified by REG. If a memory \
           reference is specified, the instructions use the byte in the \
           memory location addressed by registers Hand L. The H reg- \
           ister holds the most significant 8 bits of the address, while \
           the L register holds the least significant 8 bits of the address. 
        // Description: The specified byte is added to the con· \
           tents of the accumulator using two's complement arithmetic.
        // ADD E
        case 0x83:
            // do the math with higher precision for carry out \
               analysis
            uint16_t res = (uint16_t) state->a + (uint16_t) state->e;
            // zero flag
            if ((res & 0b11111111) == 0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (res & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            if (res > 0b11111111)
                state->cc.cy = 1;
            else
                state->cc.cy = 0;
            
            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;
            
            break;

        // Instructions in this class operate on the accumulator \
           using the byte in the register specified by REG. If a memory \
           reference is specified, the instructions use the byte in the \
           memory location addressed by registers Hand L. The H reg- \
           ister holds the most significant 8 bits of the address, while \
           the L register holds the least significant 8 bits of the address. 
        // Description: The specified byte is added to the con· \
           tents of the accumulator using two's complement arithmetic.
        // ADD H
        case 0x84: 
            // do the math with higher precision for carry out \
               analysis
            uint16_t res = (uint16_t) state->a + (uint16_t) state->h;
            // zero flag
            if ((res & 0b11111111) == 0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (res & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            if (res > 0b11111111)
                state->cc.cy = 1;
            else
                state->cc.cy = 0;
            
            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;
            
            break;

        // Instructions in this class operate on the accumulator \
           using the byte in the register specified by REG. If a memory \
           reference is specified, the instructions use the byte in the \
           memory location addressed by registers Hand L. The H reg- \
           ister holds the most significant 8 bits of the address, while \
           the L register holds the least significant 8 bits of the address. 
        // Description: The specified byte is added to the con· \
           tents of the accumulator using two's complement arithmetic.
        // ADD L
        case 0x85:
            // do the math with higher precision for carry out \
               analysis
            uint16_t res = (uint16_t) state->a + (uint16_t) state->l;
            // zero flag
            if ((res & 0b11111111) == 0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (res & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            if (res > 0b11111111)
                state->cc.cy = 1;
            else
                state->cc.cy = 0;
            
            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;
            
            break;

        // Instructions in this class operate on the accumulator \
           using the byte in the register specified by REG. If a memory \
           reference is specified, the instructions use the byte in the \
           memory location addressed by registers Hand L. The H reg- \
           ister holds the most significant 8 bits of the address, while \
           the L register holds the least significant 8 bits of the address. 
        // Description: The specified byte is added to the con· \
           tents of the accumulator using two's complement arithmetic.
        // ADD M (memory reference)
        case 0x86:
            // do the math with higher precision for carry out \
               analysis
            uint16_t res = (uint16_t) state->a + (uint16_t) state->memory[(state->h << 8) | state->l];
            // zero flag
            if ((res & 0b11111111) == 0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (res & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            if (res > 0b11111111)
                state->cc.cy = 1;
            else
                state->cc.cy = 0;
            
            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;
            
            break;

        // Instructions in this class operate on the accumulator \
           using the byte in the register specified by REG. If a memory \
           reference is specified, the instructions use the byte in the \
           memory location addressed by registers Hand L. The H reg- \
           ister holds the most significant 8 bits of the address, while \
           the L register holds the least significant 8 bits of the address. 
        // Description: The specified byte is added to the con· \
           tents of the accumulator using two's complement arithmetic.
        // ADD A
        case 0x87: UnimplementedInstruction(state); break;
            // do the math with higher precision for carry out \
               analysis
            uint16_t res = (uint16_t) state->a + (uint16_t) state->a;
            // zero flag
            if ((res & 0b11111111) == 0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (res & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            if (res > 0b11111111)
                state->cc.cy = 1;
            else
                state->cc.cy = 0;
            
            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;
            
            break;            
        // Just like ADD, but addas carry out in the LSB if set
        // ADC B
        case 0x88: 
            // do the math with higher precision for carry out \
               analysis
            uint16_t res = (uint16_t) state->a + (uint16_t) state->b;
            if (state->cc.z == 1)
                res += 0b1;

            // zero flag
            if ((res & 0b11111111) == 0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (res & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            if (res > 0b11111111)
                state->cc.cy = 1;
            else
                state->cc.cy = 0;
            
            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;
            
            break;  

        // Just like ADD, but addas carry out in the LSB if set
        // ADC C
        case 0x89: 
            // do the math with higher precision for carry out \
               analysis
            uint16_t res = (uint16_t) state->a + (uint16_t) state->c;
            if (state->cc.z == 1)
                res += 0b1;

            // zero flag
            if ((res & 0b11111111) == 0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (res & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            if (res > 0b11111111)
                state->cc.cy = 1;
            else
                state->cc.cy = 0;
            
            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;
            
            break;  

        // Just like ADD, but addas carry out in the LSB if set
        // ADC D
        case 0x8a: 
            // do the math with higher precision for carry out \
               analysis
            uint16_t res = (uint16_t) state->a + (uint16_t) state->d;
            if (state->cc.z == 1)
                res += 0b1;

            // zero flag
            if ((res & 0b11111111) == 0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (res & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            if (res > 0b11111111)
                state->cc.cy = 1;
            else
                state->cc.cy = 0;
            
            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;
            
            break;  

        // Just like ADD, but addas carry out in the LSB if set
        // ADC E
        case 0x8b: 
            // do the math with higher precision for carry out \
               analysis
            uint16_t res = (uint16_t) state->a + (uint16_t) state->e;
            if (state->cc.z == 1)
                res += 0b1;

            // zero flag
            if ((res & 0b11111111) == 0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (res & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            if (res > 0b11111111)
                state->cc.cy = 1;
            else
                state->cc.cy = 0;
            
            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;
            
            break;  

        // Just like ADD, but addas carry out in the LSB if set
        // ADC H
        case 0x8c: 
            // do the math with higher precision for carry out \
               analysis
            uint16_t res = (uint16_t) state->a + (uint16_t) state->h;
            if (state->cc.z == 1)
                res += 0b1;

            // zero flag
            if ((res & 0b11111111) == 0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (res & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            if (res > 0b11111111)
                state->cc.cy = 1;
            else
                state->cc.cy = 0;
            
            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;
            
            break;  

        // Just like ADD, but addas carry out in the LSB if set
        // ADC L
        case 0x8d: 
            // do the math with higher precision for carry out \
               analysis
            uint16_t res = (uint16_t) state->a + (uint16_t) state->l;
            if (state->cc.z == 1)
                res += 0b1;

            // zero flag
            if ((res & 0b11111111) == 0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (res & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            if (res > 0b11111111)
                state->cc.cy = 1;
            else
                state->cc.cy = 0;
            
            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;
            
            break;  

        // Just like ADD, but addas carry out in the LSB if set
        // ADC M (mem ref)
        case 0x8e: 
            // do the math with higher precision for carry out \
               analysis
            uint16_t res = (uint16_t) state->a + state->memory[(state->h << 8) | state->l];
            if (state->cc.z == 1)
                res += 0b1;

            // zero flag
            if ((res & 0b11111111) == 0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (res & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            if (res > 0b11111111)
                state->cc.cy = 1;
            else
                state->cc.cy = 0;
            
            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;
            
            break;  

        // Just like ADD, but addas carry out in the LSB if set
        // ADC A
        case 0x8f:
            // do the math with higher precision for carry out \
               analysis
            uint16_t res = (uint16_t) state->a + (uint16_t) state->a;
            if (state->cc.z == 1)
                res += 0b1;

            // zero flag
            if ((res & 0b11111111) == 0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (res & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            if (res > 0b11111111)
                state->cc.cy = 1;
            else
                state->cc.cy = 0;
            
            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;
            
            break; 
        
        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB B
        case 0x90: 
            uint16_t res = (uint16_t) state->a - (uint16_t) state->b;

            // zero flag
            if ((res & 0b11111111) == 0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (res & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            // there will be carry if the result is 0 or negative
            // if there is carry, carry flag is reset
            // if there isn't carry, carry flag is set
            state->cc.cy = (state->a <= state->b) ? 0 : 1;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res??????????????????????

        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB C
        case 0x91: UnimplementedInstruction(state); break;

        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB D
        case 0x92: UnimplementedInstruction(state); break;

        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB E
        case 0x93: UnimplementedInstruction(state); break;

        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB H
        case 0x94: UnimplementedInstruction(state); break;

        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB L
        case 0x95: UnimplementedInstruction(state); break;

        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB M (mem ref)
        case 0x96: UnimplementedInstruction(state); break;

        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB A
        case 0x97: UnimplementedInstruction(state); break;
        case 0x98: UnimplementedInstruction(state); break;
        case 0x99: UnimplementedInstruction(state); break;
        case 0x9a: UnimplementedInstruction(state); break;
        case 0x9b: UnimplementedInstruction(state); break;
        case 0x9c: UnimplementedInstruction(state); break;
        case 0x9d: UnimplementedInstruction(state); break;
        case 0x9e: UnimplementedInstruction(state); break;
        case 0x9f: UnimplementedInstruction(state); break;
        case 0xa0: UnimplementedInstruction(state); break;
        case 0xa1: UnimplementedInstruction(state); break;
        case 0xa2: UnimplementedInstruction(state); break;
        case 0xa3: UnimplementedInstruction(state); break;
        case 0xa4: UnimplementedInstruction(state); break;
        case 0xa5: UnimplementedInstruction(state); break;
        case 0xa6: UnimplementedInstruction(state); break;
        case 0xa7: UnimplementedInstruction(state); break;
        case 0xa8: UnimplementedInstruction(state); break;
        case 0xa9: UnimplementedInstruction(state); break;
        case 0xaa: UnimplementedInstruction(state); break;
        case 0xab: UnimplementedInstruction(state); break;
        case 0xac: UnimplementedInstruction(state); break;
        case 0xad: UnimplementedInstruction(state); break;
        case 0xae: UnimplementedInstruction(state); break;
        case 0xaf: UnimplementedInstruction(state); break;
        case 0xb0: UnimplementedInstruction(state); break;
        case 0xb1: UnimplementedInstruction(state); break;
        case 0xb2: UnimplementedInstruction(state); break;
        case 0xb3: UnimplementedInstruction(state); break;
        case 0xb4: UnimplementedInstruction(state); break;
        case 0xb5: UnimplementedInstruction(state); break;
        case 0xb6: UnimplementedInstruction(state); break;
        case 0xb7: UnimplementedInstruction(state); break;
        case 0xb8: UnimplementedInstruction(state); break;
        case 0xb9: UnimplementedInstruction(state); break;
        case 0xba: UnimplementedInstruction(state); break;
        case 0xbb: UnimplementedInstruction(state); break;
        case 0xbc: UnimplementedInstruction(state); break;
        case 0xbd: UnimplementedInstruction(state); break;
        case 0xbe: UnimplementedInstruction(state); break;
        case 0xbf: UnimplementedInstruction(state); break;
        case 0xc0: UnimplementedInstruction(state); break;
        case 0xc1: UnimplementedInstruction(state); break;
        case 0xc2: UnimplementedInstruction(state); break;
        case 0xc3: UnimplementedInstruction(state); break;
        case 0xc4: UnimplementedInstruction(state); break;
        case 0xc5: UnimplementedInstruction(state); break;

        // Description: The byte of immediate data is added to \
           the contents of the accumulator using two's complement \
           arithmetic.
        // ADI
        case 0xc6: 
            
            break;
        case 0xc7: UnimplementedInstruction(state); break;
        case 0xc8: UnimplementedInstruction(state); break;
        case 0xc9: UnimplementedInstruction(state); break;
        case 0xca: UnimplementedInstruction(state); break;
        case 0xcb: UnimplementedInstruction(state); break;
        case 0xcc: UnimplementedInstruction(state); break;
        case 0xcd: UnimplementedInstruction(state); break;
        case 0xce: UnimplementedInstruction(state); break;
        case 0xcf: UnimplementedInstruction(state); break;
        case 0xd0: UnimplementedInstruction(state); break;
        case 0xd1: UnimplementedInstruction(state); break;
        case 0xd2: UnimplementedInstruction(state); break;
        case 0xd3: UnimplementedInstruction(state); break;
        case 0xd4: UnimplementedInstruction(state); break;
        case 0xd5: UnimplementedInstruction(state); break;
        case 0xd6: UnimplementedInstruction(state); break;
        case 0xd7: UnimplementedInstruction(state); break;
        case 0xd8: UnimplementedInstruction(state); break;
        case 0xd9: UnimplementedInstruction(state); break;
        case 0xda: UnimplementedInstruction(state); break;
        case 0xdb: UnimplementedInstruction(state); break;
        case 0xdc: UnimplementedInstruction(state); break;
        case 0xdd: UnimplementedInstruction(state); break;
        case 0xde: UnimplementedInstruction(state); break;
        case 0xdf: UnimplementedInstruction(state); break;
        case 0xe0: UnimplementedInstruction(state); break;
        case 0xe1: UnimplementedInstruction(state); break;
        case 0xe2: UnimplementedInstruction(state); break;
        case 0xe3: UnimplementedInstruction(state); break;
        case 0xe4: UnimplementedInstruction(state); break;
        case 0xe5: UnimplementedInstruction(state); break;
        case 0xe6: UnimplementedInstruction(state); break;
        case 0xe7: UnimplementedInstruction(state); break;
        case 0xe8: UnimplementedInstruction(state); break;
        case 0xe9: UnimplementedInstruction(state); break;
        case 0xea: UnimplementedInstruction(state); break;
        case 0xeb: UnimplementedInstruction(state); break;
        case 0xec: UnimplementedInstruction(state); break;
        case 0xed: UnimplementedInstruction(state); break;
        case 0xee: UnimplementedInstruction(state); break;
        case 0xef: UnimplementedInstruction(state); break;
        case 0xf0: UnimplementedInstruction(state); break;
        case 0xf1: UnimplementedInstruction(state); break;
        case 0xf2: UnimplementedInstruction(state); break;
        case 0xf3: UnimplementedInstruction(state); break;
        case 0xf4: UnimplementedInstruction(state); break;
        case 0xf5: UnimplementedInstruction(state); break;
        case 0xf6: UnimplementedInstruction(state); break;
        case 0xf7: UnimplementedInstruction(state); break;
        case 0xf8: UnimplementedInstruction(state); break;
        case 0xf9: UnimplementedInstruction(state); break;
        case 0xfa: UnimplementedInstruction(state); break;
        case 0xfb: UnimplementedInstruction(state); break;
        case 0xfc: UnimplementedInstruction(state); break;
        case 0xfd: UnimplementedInstruction(state); break;
        case 0xfe: UnimplementedInstruction(state); break;
        case 0xff: UnimplementedInstruction(state); break;
    }
    state->pc++;


}