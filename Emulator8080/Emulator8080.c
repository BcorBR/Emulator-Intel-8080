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

        // INR Increment Register or Memory
        // Description: The specified register or memory byte is \
           incremented by one.
        // INR B
        case 0x04:
            state->b += 1;
            break;

        // DCR Decrement Register or Memory
        // Description: The specified register or memory byte is \
           decremented by one.
        // DCR B
        case 0x05:
            state->b -= 1;
            break;
        
        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI B
        case 0x06: 
            state->b = opcode[1];
            state->pc += 1;
            break;

        // RLC Rotate Accumulator Left
        // Description: The Carry bit is set equal to the high- \
           order bit of the accumulator. The contents of the accumu-\
           lator are rotated one bit position to the left, with the high- \
           order bit being transferred to the low-order bit position of \
           the accumulator.
        // RLC
        case 0x07:
            state->cc.cy = (state->a >> 7) & 0b1;
            state->a = (state->a << 1) | state->cc.cy;
            break;

        case 0x08: UnimplementedInstruction(state); break;
        case 0x09: UnimplementedInstruction(state); break;
        case 0x0a: UnimplementedInstruction(state); break;
        case 0x0b: UnimplementedInstruction(state); break;

        // INR Increment Register or Memory
        // Description: The specified register or memory byte is \
           incremented by one.
        // INR C
        case 0x0c: 
            state->c += 1;
            break;

        // DCR Decrement Register or Memory
        // Description: The specified register or memory byte is \
           decremented by one.
        // DCR C
        case 0x0d:
            state->c -= 1;
            break;

        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI C
        case 0x0e: 
            state->c = opcode[1];
            state->pc += 1;
            break;

        // RRC Rotate Accumulator Right
        // RRC
        case 0x0f:
            state->cc.cy = (state->a & 0b1);
            state-> a = (state->a >> 1) | state->cc.cy;
            break;

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

        // INR Increment Register or Memory
        // Description: The specified register or memory byte is \
           incremented by one.
        // INR D
        case 0x14:
            state->d += 1;
            break;

        // DCR Decrement Register or Memory
        // Description: The specified register or memory byte is \
           decremented by one.
        // DCR D
        case 0x15:
            state->d -= 1;
            break;

        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI D
        case 0x16: 
            state->d = opcode[1];
            state->pc += 1;
            break;

        // RAL Rotate Accumulator Left Through Carry
        // RAL
        case 0x17: 
            uint8_t tmp = state->cc.cy;
            state->cc.cy = (state->a >> 7) & 0b1;
            state->a = (state->a << 1) | tmp;
            break;

        case 0x18: UnimplementedInstruction(state); break;
        case 0x19: UnimplementedInstruction(state); break;
        case 0x1a: UnimplementedInstruction(state); break;
        case 0x1b: UnimplementedInstruction(state); break;

        // INR Increment Register or Memory
        // Description: The specified register or memory byte is \
           incremented by one.
        // INR E
        case 0x1c:
            state->e += 1;
            break;

        // DCR Decrement Register or Memory
        // Description: The specified register or memory byte is \
           decremented by one.
        // DCR E
        case 0x1d:
            state->e -= 1;
            break;

        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI E
        case 0x1e: 
            state->e = opcode[1];
            state->pc += 1;
            break;
        
        // RAR Rotate Accumulator Right Through Carry
        // RAR
        case 0x1f:
            uint8_t tmp = state->cc.cy;
            state->cc.cy = state->a & 0b1;
            state->a = (state->a >> 1) | (tmp << 7);
            break;
            
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

        // SHLD Store Hand L Direct
        // Description: The contents of the L register are stored \
           at the memory address formed by concatenati ng HI AD 0 \
           with LOW ADO. The contents of the H register are stored at \
           the next higher memory address.
        // SHLD
        case 0x22: 
            state->memory[(opcode[2] << 8) | opcode[1]] = state->l;
            state->memory[(opcode[2] << 8) | opcode[1] + 1] = state->h;
            state->pc += 2;
            break;

        case 0x23: UnimplementedInstruction(state); break;

        // INR Increment Register or Memory
        // Description: The specified register or memory byte is \
           incremented by one.
        // INR H
        case 0x24:
            state->h += 1;
            break;

        // DCR Decrement Register or Memory
        // Description: The specified register or memory byte is \
           decremented by one.
        // DCR H
        case 0x25:
            state->h -= 1;
            break;

        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI H
        case 0x26: 
            state->h = opcode[1];
            state->pc += 1;
            break;
        
        // NOT IMPLEMENTED
        case 0x27: UnimplementedInstruction(state); break;

        case 0x28: UnimplementedInstruction(state); break;
        case 0x29: UnimplementedInstruction(state); break;

        // LHLD Load HAnd L Direct
        // Description: The byte at the memory address formed \
           by concatenating HI ADD with LOW ADD replaces the con- \
           tents of the L register. The byte at the next higher memory \
           address replaces the contents of the H register
        // LHLD
        case 0x2a: 
            state->l = (opcode[2] << 8) | opcode[1];
            state->h = (opcode[2] << 8) | opcode[1] + 1;
            state->pc += 2;
            break;

        // INR Increment Register or Memory
        // Description: The specified register or memory byte is \
           incremented by one.
        // INR L
        case 0x2c:
            state->l += 1;
            break;

        case 0x2b: UnimplementedInstruction(state); break;

        // DCR Decrement Register or Memory
        // Description: The specified register or memory byte is \
           decremented by one.
        // DCR L
        case 0x2d:
            state->l -= 1;
            break;

        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI L
        case 0x2e: 
            state->l = opcode[1];
            state->pc += 1;
            break;
        
        // CMA Complement Accumulator
        // Description: Each bit of the contents of the accumula- \
           tor is complemented (producing the one's complement)
        // CMA
        case 0x2f:
            state->a = ~state->a;
            break;

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

        // STA Store Accumulator Direct
        // Description: The contents of the accumulator replace \
           the byte at the memory address formed by concatenating \
           HI ADO with LOW ADO.
        // STA
        case 0x32: 
            state->memory[(opcode[2] << 8) | opcode[1]] = state->a;
            state->pc += 2;
            break;

        case 0x33: UnimplementedInstruction(state); break;

        // INR Increment Register or Memory
        // Description: The specified register or memory byte is \
           incremented by one.
        // INR M (mem ref)
        case 0x34: 
            state->memory[((state->h) << 8) | state->l] += 1;
            break;

        // DCR Decrement Register or Memory
        // Description: The specified register or memory byte is \
           decremented by one.
        // DCR M (mem ref)
        case 0x35: 
            state->memory[((state->h) << 8) | state->l] -= 1;
            break;

        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI ref M (H + L)
        case 0x36: 
            state->memory[(state->h << 8) | state->l] = opcode[1];
            state->pc += 1;
            break;

        // STC Set Carry
        // Description: The Carry bit is set to one.
        // STC
        case 0x37:
            state->cc.cy = 1;
            break;

        case 0x38: UnimplementedInstruction(state); break;
        case 0x39: UnimplementedInstruction(state); break;

        // LDA Load Accumulator Direct
        // Description: The byte at the memory address formed \
           by concatenating HI ADD with LOW ADD replaces the con- \
           tents of the accumulator.
        // LDA
        case 0x3a:
            state->a = state->memory[(opcode[2] << 8) | opcode[1]];
            state->pc += 2;
            break;

        case 0x3b: UnimplementedInstruction(state); break;

        // INR Increment Register or Memory
        // Description: The specified register or memory byte is \
           incremented by one.
        // INR A
        case 0x3c:
            state->a += 1;
            break;

        // DCR Decrement Register or Memory
        // Description: The specified register or memory byte is \
           decremented by one.
        // DCR A
        case 0x3d:
            state->a -= 1;
            break;

        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI A
        case 0x3e: 
            state->a = opcode[1];
            state->pc += 1;
            break;

        // CMC Complement Carry
        // Description: If the Carry bit = 0, it is set to 1. If the Carry \
           bit = 1, it is reset to O.
        // CMC
        case 0x3f:
            if (state->cc.cy)
                state->cc.cy = 0;
            else   
                state->cc.cy = 1;
            
            break;
        
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
            uint16_t res = (uint16_t) state->a + (uint16_t) state->b + state->cc.cy;

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
            uint16_t res = (uint16_t) state->a + (uint16_t) state->c + state->cc.cy;

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
            uint16_t res = (uint16_t) state->a + (uint16_t) state->d + state->cc.cy;

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
            uint16_t res = (uint16_t) state->a + (uint16_t) state->e + state->cc.cy;

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
            uint16_t res = (uint16_t) state->a + (uint16_t) state->h + state->cc.cy;

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
            uint16_t res = (uint16_t) state->a + (uint16_t) state->l + state->cc.cy;

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
            uint16_t res = (uint16_t) state->a + state->memory[(state->h << 8) | state->l] + state->cc.cy;

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
            uint16_t res = (uint16_t) state->a + (uint16_t) state->a + state->cc.cy;

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
            state->a = res & 0b11111111;

            break;

        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB C
        case 0x91: 
            uint16_t res = (uint16_t) state->a - (uint16_t) state->c;

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
            state->a = res & 0b11111111;

            break;

        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB D
        case 0x92: 
            uint16_t res = (uint16_t) state->a - (uint16_t) state->d;

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
            state->a = res & 0b11111111;

            break;

        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB E
        case 0x93: 
            uint16_t res = (uint16_t) state->a - (uint16_t) state->e;

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
            state->a = res & 0b11111111;

            break;

        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB H
        case 0x94: 
            uint16_t res = (uint16_t) state->a - (uint16_t) state->h;

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
            state->a = res & 0b11111111;

            break;

        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB L
        case 0x95: 
            uint16_t res = (uint16_t) state->a - (uint16_t) state->l;

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
            state->a = res & 0b11111111;

            break;

        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB M (mem ref)
        case 0x96: 
            uint16_t res = (uint16_t) state->a - state->memory[(state->h << 8) | state->l];

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
            state->a = res & 0b11111111;

            break;

        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB A
        case 0x97: 
            uint16_t res = (uint16_t) state->a - (uint16_t) state->a;

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
            state->a = res & 0b11111111;

            break;

        // SBB Subtract Register or Memory From \
           Accumulator With Borrow
        // Description: The Carry bit is internally added to the \
           contents of the specified byte. This value is then subtracted \
           from the accumulator using two's complement arithmetic.
        // SBB B
        case 0x98: 
            uint16_t res = (uint16_t) state->a - ((uint16_t) state->b + state->cc.cy);

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
            state->a = res & 0b11111111;

            break;

        // SBB Subtract Register or Memory From \
           Accumulator With Borrow
        // Description: The Carry bit is internally added to the \
           contents of the specified byte. This value is then subtracted \
           from the accumulator using two's complement arithmetic.
        // SBB C
        case 0x99:
            uint16_t res = (uint16_t) state->a - ((uint16_t) state->c + state->cc.cy);

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
            state->a = res & 0b11111111;

        // SBB Subtract Register or Memory From \
           Accumulator With Borrow
        // Description: The Carry bit is internally added to the \
           contents of the specified byte. This value is then subtracted \
           from the accumulator using two's complement arithmetic.
        // SBB D
        case 0x9a: 
            uint16_t res = (uint16_t) state->a - ((uint16_t) state->d + state->cc.cy);

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
            state->a = res & 0b11111111;

            break;

        // SBB Subtract Register or Memory From \
           Accumulator With Borrow
        // Description: The Carry bit is internally added to the \
           contents of the specified byte. This value is then subtracted \
           from the accumulator using two's complement arithmetic.
        // SBB E
        case 0x9b: 
            uint16_t res = (uint16_t) state->a - ((uint16_t) state->e + state->cc.cy);

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
            state->a = res & 0b11111111;

            break;

        // SBB Subtract Register or Memory From \
           Accumulator With Borrow
        // Description: The Carry bit is internally added to the \
           contents of the specified byte. This value is then subtracted \
           from the accumulator using two's complement arithmetic.
        // SBB H
        case 0x9c:
            uint16_t res = (uint16_t) state->a - ((uint16_t) state->h + state->cc.cy);

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
            state->a = res & 0b11111111;

            break;

        // SBB Subtract Register or Memory From \
           Accumulator With Borrow
        // Description: The Carry bit is internally added to the \
           contents of the specified byte. This value is then subtracted \
           from the accumulator using two's complement arithmetic.
        // SBB L
        case 0x9d: 
            uint16_t res = (uint16_t) state->a - ((uint16_t) state->l + state->cc.cy);

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
            state->a = res & 0b11111111;

            break;

        // SBB Subtract Register or Memory From \
           Accumulator With Borrow
        // Description: The Carry bit is internally added to the \
           contents of the specified byte. This value is then subtracted \
           from the accumulator using two's complement arithmetic.
        // SBB M (mem ref)
        case 0x9e: 
            uint16_t res = (uint16_t) state->a - (state->memory[(state->h << 8) | state->l] + state->cc.cy);   // ((uint16_t) state->b + state->cc.cy);

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
            state->a = res & 0b11111111;

            break;

        // SBB Subtract Register or Memory From \
           Accumulator With Borrow
        // Description: The Carry bit is internally added to the \
           contents of the specified byte. This value is then subtracted \
           from the accumulator using two's complement arithmetic.
        // SBB A
        case 0x9f: 
            uint16_t res = (uint16_t) state->a - ((uint16_t) state->a + state->cc.cy);

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
            state->a = res & 0b11111111;

            break;

        // ANA Logical and Register or Memory \
           With Accumulator
        // Description: The specified byte is logically ANDed bit \
           by bit with the contents of the accumulator. The Carry bit \
           is reset to zero.
        // ANA B
        case 0xa0:
            state->a = state->a & state->b;
            
            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;

            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // ANA Logical and Register or Memory \
           With Accumulator
        // Description: The specified byte is logically ANDed bit \
           by bit with the contents of the accumulator. The Carry bit \
           is reset to zero.
        // ANA C
        case 0xa1: 
            state->a = state->a & state->c;
            
            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;

            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // ANA Logical and Register or Memory \
           With Accumulator
        // Description: The specified byte is logically ANDed bit \
           by bit with the contents of the accumulator. The Carry bit \
           is reset to zero.
        // ANA D
        case 0xa2: 
            state->a = state->a & state->d;
            
            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;

            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // ANA Logical and Register or Memory \
           With Accumulator
        // Description: The specified byte is logically ANDed bit \
           by bit with the contents of the accumulator. The Carry bit \
           is reset to zero.
        // ANA E
        case 0xa3: 
            state->a = state->a & state->e;
            
            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;

            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // ANA Logical and Register or Memory \
           With Accumulator
        // Description: The specified byte is logically ANDed bit \
           by bit with the contents of the accumulator. The Carry bit \
           is reset to zero.
        // ANA H
        case 0xa4: 
            state->a = state->a & state->h;
            
            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;

            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // ANA Logical and Register or Memory \
           With Accumulator
        // Description: The specified byte is logically ANDed bit \
           by bit with the contents of the accumulator. The Carry bit \
           is reset to zero.
        // ANA L
        case 0xa5:
            state->a = state->a & state->l;
            
            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;

            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // ANA Logical and Register or Memory \
           With Accumulator
        // Description: The specified byte is logically ANDed bit \
           by bit with the contents of the accumulator. The Carry bit \
           is reset to zero.
        // ANA M (mem ref)
        case 0xa6: 
            state->a = state->a & state->memory[(state->h << 8) | state->l];
            
            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;

            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // ANA Logical and Register or Memory \
           With Accumulator
        // Description: The specified byte is logically ANDed bit \
           by bit with the contents of the accumulator. The Carry bit \
           is reset to zero.
        // ANA A
        case 0xa7: 
            state->a = state->a & state->a;
            
            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;

            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // XRA Logical Exclusive-Or Register or Memory \
           With Accumulator (Zero Accumulator)
        // Description: The specified byte is EXCLUSIVE-ORed \
           bit by bit with the contents of the accumulator. The Carry \
           bit is reset to zero.
        // XRA B
        case 0xa8:
            state->a = state->a ^ state->b;

            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // XRA Logical Exclusive-Or Register or Memory \
           With Accumulator (Zero Accumulator)
        // Description: The specified byte is EXCLUSIVE-ORed \
           bit by bit with the contents of the accumulator. The Carry \
           bit is reset to zero.
        // XRA C
        case 0xa9:
            state->a = state->a ^ state->c;

            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // XRA Logical Exclusive-Or Register or Memory \
           With Accumulator (Zero Accumulator)
        // Description: The specified byte is EXCLUSIVE-ORed \
           bit by bit with the contents of the accumulator. The Carry \
           bit is reset to zero.
        // XRA D
        case 0xaa: 
            state->a = state->a ^ state->d;

            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // XRA Logical Exclusive-Or Register or Memory \
           With Accumulator (Zero Accumulator)
        // Description: The specified byte is EXCLUSIVE-ORed \
           bit by bit with the contents of the accumulator. The Carry \
           bit is reset to zero.
        // XRA E
        case 0xab: 
            state->a = state->a ^ state->e;

            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // XRA Logical Exclusive-Or Register or Memory \
           With Accumulator (Zero Accumulator)
        // Description: The specified byte is EXCLUSIVE-ORed \
           bit by bit with the contents of the accumulator. The Carry \
           bit is reset to zero.
        // XRA H
        case 0xac: 
            state->a = state->a ^ state->h;

            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // XRA Logical Exclusive-Or Register or Memory \
           With Accumulator (Zero Accumulator)
        // Description: The specified byte is EXCLUSIVE-ORed \
           bit by bit with the contents of the accumulator. The Carry \
           bit is reset to zero.
        // XRA L
        case 0xad: 
            state->a = state->a ^ state->l;

            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // XRA Logical Exclusive-Or Register or Memory \
           With Accumulator (Zero Accumulator)
        // Description: The specified byte is EXCLUSIVE-ORed \
           bit by bit with the contents of the accumulator. The Carry \
           bit is reset to zero.
        // XRA M (mem ref)
        case 0xae: 
            state->a = state->a ^ state->memory[(state->h << 8) | state->l];

            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // XRA Logical Exclusive-Or Register or Memory \
           With Accumulator (Zero Accumulator)
        // Description: The specified byte is EXCLUSIVE-ORed \
           bit by bit with the contents of the accumulator. The Carry \
           bit is reset to zero.
        // XRA A
        case 0xaf: 
            state->a = state->a ^ state->a;

            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // ORA Logical or Register or Memory With \
           Accumulator
        // Description: The specified byte is logically ORed bit \
           by bit with the contents of the accumulator. The carry bit \
           is reset to zero.
        // ORA B
        case 0xb0: 
            state->a = state->a | state->b;

            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;

            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;

            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // ORA Logical or Register or Memory With \
           Accumulator
        // Description: The specified byte is logically ORed bit \
           by bit with the contents of the accumulator. The carry bit \
           is reset to zero.
        // ORA C
        case 0xb1: 
            state->a = state->a | state->c;

            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;

            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;

            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // ORA Logical or Register or Memory With \
           Accumulator
        // Description: The specified byte is logically ORed bit \
           by bit with the contents of the accumulator. The carry bit \
           is reset to zero.
        // ORA D
        case 0xb2: 
            state->a = state->a | state->d;

            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;

            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;

            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // ORA Logical or Register or Memory With \
           Accumulator
        // Description: The specified byte is logically ORed bit \
           by bit with the contents of the accumulator. The carry bit \
           is reset to zero.
        // ORA E
        case 0xb3: 
            state->a = state->a | state->e;

            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;

            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;

            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // ORA Logical or Register or Memory With \
           Accumulator
        // Description: The specified byte is logically ORed bit \
           by bit with the contents of the accumulator. The carry bit \
           is reset to zero.
        // ORA H
        case 0xb4: 
            state->a = state->a | state->h;

            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;

            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;

            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // ORA Logical or Register or Memory With \
           Accumulator
        // Description: The specified byte is logically ORed bit \
           by bit with the contents of the accumulator. The carry bit \
           is reset to zero.
        // ORA L
        case 0xb5: 
            state->a = state->a | state->l;

            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;

            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;

            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // ORA Logical or Register or Memory With \
           Accumulator
        // Description: The specified byte is logically ORed bit \
           by bit with the contents of the accumulator. The carry bit \
           is reset to zero.
        // ORA M (mem ref)
        case 0xb6: 
            state->a = state->a | state->memory[(state->h << 8) | state->l];

            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;

            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;

            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // ORA Logical or Register or Memory With \
           Accumulator
        // Description: The specified byte is logically ORed bit \
           by bit with the contents of the accumulator. The carry bit \
           is reset to zero.
        // ORA A
        case 0xb7:
            state->a = state->a | state->a;

            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;

            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;

            // carry flag
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            break;

        // CMP Compare Register or Memory \
           With Accumulator
        // Description: The specified byte is compared to the \
           contents of the accumulator. The comparison is performed \
           by internally subtracting the contents of REG from the ac- \
           cumulator (leaving both unchanged) and setting the condi- \
           tion bits according to the result. In particular, the Zero bit is \
           set if the quantities are equal, and reset if they are unequal. \
           Since a subtract operation is performed, the Carry bit will be \
           set if there is no carry out of bit 7, indicati ng that the \
           contents of REG are greater than the contents of the accu- \
           mulator, and reset otherwise.
        // CMP B
        case 0xb8: 
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

            break;

        // CMP Compare Register or Memory \
           With Accumulator
        // Description: The specified byte is compared to the \
           contents of the accumulator. The comparison is performed \
           by internally subtracting the contents of REG from the ac- \
           cumulator (leaving both unchanged) and setting the condi- \
           tion bits according to the result. In particular, the Zero bit is \
           set if the quantities are equal, and reset if they are unequal. \
           Since a subtract operation is performed, the Carry bit will be \
           set if there is no carry out of bit 7, indicati ng that the \
           contents of REG are greater than the contents of the accu- \
           mulator, and reset otherwise.
        // CMP C
        case 0xb9: 
            uint16_t res = (uint16_t) state->a - (uint16_t) state->c;

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
            state->cc.cy = (state->a <= state->c) ? 0 : 1;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            break;

        // CMP Compare Register or Memory \
           With Accumulator
        // Description: The specified byte is compared to the \
           contents of the accumulator. The comparison is performed \
           by internally subtracting the contents of REG from the ac- \
           cumulator (leaving both unchanged) and setting the condi- \
           tion bits according to the result. In particular, the Zero bit is \
           set if the quantities are equal, and reset if they are unequal. \
           Since a subtract operation is performed, the Carry bit will be \
           set if there is no carry out of bit 7, indicati ng that the \
           contents of REG are greater than the contents of the accu- \
           mulator, and reset otherwise.
        // CMP D
        case 0xba: 
            uint16_t res = (uint16_t) state->a - (uint16_t) state->d;

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
            state->cc.cy = (state->a <= state->d) ? 0 : 1;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            break;

        // CMP Compare Register or Memory \
           With Accumulator
        // Description: The specified byte is compared to the \
           contents of the accumulator. The comparison is performed \
           by internally subtracting the contents of REG from the ac- \
           cumulator (leaving both unchanged) and setting the condi- \
           tion bits according to the result. In particular, the Zero bit is \
           set if the quantities are equal, and reset if they are unequal. \
           Since a subtract operation is performed, the Carry bit will be \
           set if there is no carry out of bit 7, indicati ng that the \
           contents of REG are greater than the contents of the accu- \
           mulator, and reset otherwise.
        // CMP E
        case 0xbb:
            uint16_t res = (uint16_t) state->a - (uint16_t) state->e;

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
            state->cc.cy = (state->a <= state->e) ? 0 : 1;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            break;

        // CMP Compare Register or Memory \
           With Accumulator
        // Description: The specified byte is compared to the \
           contents of the accumulator. The comparison is performed \
           by internally subtracting the contents of REG from the ac- \
           cumulator (leaving both unchanged) and setting the condi- \
           tion bits according to the result. In particular, the Zero bit is \
           set if the quantities are equal, and reset if they are unequal. \
           Since a subtract operation is performed, the Carry bit will be \
           set if there is no carry out of bit 7, indicati ng that the \
           contents of REG are greater than the contents of the accu- \
           mulator, and reset otherwise.
        // CMP H
        case 0xbc: 
            uint16_t res = (uint16_t) state->a - (uint16_t) state->h;

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
            state->cc.cy = (state->a <= state->h) ? 0 : 1;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            break;

        // CMP Compare Register or Memory \
           With Accumulator
        // Description: The specified byte is compared to the \
           contents of the accumulator. The comparison is performed \
           by internally subtracting the contents of REG from the ac- \
           cumulator (leaving both unchanged) and setting the condi- \
           tion bits according to the result. In particular, the Zero bit is \
           set if the quantities are equal, and reset if they are unequal. \
           Since a subtract operation is performed, the Carry bit will be \
           set if there is no carry out of bit 7, indicati ng that the \
           contents of REG are greater than the contents of the accu- \
           mulator, and reset otherwise.
        // CMP L
        case 0xbd: 
            uint16_t res = (uint16_t) state->a - (uint16_t) state->l;

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
            state->cc.cy = (state->a <= state->l) ? 0 : 1;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            break;

        // CMP Compare Register or Memory \
           With Accumulator
        // Description: The specified byte is compared to the \
           contents of the accumulator. The comparison is performed \
           by internally subtracting the contents of REG from the ac- \
           cumulator (leaving both unchanged) and setting the condi- \
           tion bits according to the result. In particular, the Zero bit is \
           set if the quantities are equal, and reset if they are unequal. \
           Since a subtract operation is performed, the Carry bit will be \
           set if there is no carry out of bit 7, indicati ng that the \
           contents of REG are greater than the contents of the accu- \
           mulator, and reset otherwise.
        // CMP M (mem ref)
        case 0xbe: 
            uint16_t res = (uint16_t) state->a - (uint16_t) state->memory[(state->h << 8) | state->l];

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
            state->cc.cy = (state->a <= state->memory[(state->h << 8) | state->l]) ? 0 : 1;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            break;

        // CMP Compare Register or Memory \
           With Accumulator
        // Description: The specified byte is compared to the \
           contents of the accumulator. The comparison is performed \
           by internally subtracting the contents of REG from the ac- \
           cumulator (leaving both unchanged) and setting the condi- \
           tion bits according to the result. In particular, the Zero bit is \
           set if the quantities are equal, and reset if they are unequal. \
           Since a subtract operation is performed, the Carry bit will be \
           set if there is no carry out of bit 7, indicati ng that the \
           contents of REG are greater than the contents of the accu- \
           mulator, and reset otherwise.
        // CMP A
        case 0xbf:
            uint16_t res = (uint16_t) state->a - (uint16_t) state->a;

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
            state->cc.cy = (state->a <= state->a) ? 0 : 1;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            break;

        // RNZ Return If Not Zero
        // Description: If the Zero bit is zero, a return operation \
           is performed.
        // RNZ
        case 0xc0:
            if (!(state->cc.z)){
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
                state->sp += 2;
            }

            break;

        case 0xc1: UnimplementedInstruction(state); break;

        // JNZ Jump If Not Zero 
        // Description: If the Zero bit is zero, program execu- \
           tion continues at the memory address adr.
        // JNZ
        case 0xc2:
            if (!(state->cc.z))
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc += 2;
            
            break;

        // JMP JUMP
        // Description: Program execution continues uncondi- \
           tionally at memory address adr.
        // JMP
        case 0xc3:
            state->pc =(opcode[2] << 8) | opcode[1];

            break;

        // CNZ Call If Not Zero
        // Description: If the Zero bit is one, a call operation is \
           performed to subroutine sub.
        // CNZ
        case 0xc4:
            if (state->cc.z){
                state->memory[state->sp-1] = ((state->pc+2) >> 8) & 0b11111111;
                state->memory[state->sp-2] = (state->pc+2) & 0b11111111;
                state->sp -= 2;
                state->pc = ((opcode[2]) << 8) | opcode[1];
            }
            else
                state->pc += 2;
            
            break;

        case 0xc5: UnimplementedInstruction(state); break;

        // Description: The byte of immediate data is added to \
           the contents of the accumulator using two's complement \
           arithmetic.
        // ADI
        case 0xc6: 
            uint16_t res = (uint16_t) state->a + (uint16_t) opcode[1];
            
            // zero flag
            if ((res & 0b11111111) == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            if (res > 0b11111111)
                state->cc.cy = 1;
            else
                state->cc.cy = 0;

            state->cc.p = parity(res & 0b11111111);

            state->a = res & 0b11111111;            

            state->pc += 1;

            break;
        case 0xc7: UnimplementedInstruction(state); break;
        
        // RZ Return If Zero
        // Description: If the Zero bit is one, a return operation \
           is performed.
        // RZ   
        case 0xc8:
            if (state->cc.z){
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
                state->sp += 2;
            }

            break;

        // RET Return
        // Description: A return operation is unconditionally \
           performed.
        // RET
        case 0xc9:
            state->pc = state->memory[state->sp] | (state->memory[state->sp-1] << 8);
            state->sp += 2;
            break;

        // JZ Jump If Zero
        // Description: If the zero bit is one, program execution \
           continues at the memory address adr.
        // JZ
        case 0xca:
            if (state->cc.z)
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc += 2;
            
            break;

        case 0xcb: UnimplementedInstruction(state); break;

        // CZ Call If Zero
        // Description: If the Zero bit is zero, a call operation is \
           performed to subroutine sub.
        // CZ
        case 0xcc:
            if (!(state->cc.z)){
                state->memory[state->sp-1] = ((state->pc+2) >> 8) & 0b11111111;
                state->memory[state->sp-2] = (state->pc+2) & 0b11111111;
                state->sp -= 2;
                state->pc = (opcode[2] << 8) | opcode[1];
            }
            else
                state->pc += 2;
            break;

        // CALL Call
        // Description: A call operation is unconditionally per- \
           formed to subroutine sub.
        // CALL
        case 0xcd:
            state->memory[state->sp - 1] = ((state->pc + 2) >> 8) & 0b11111111;
            state->memory[state->sp - 2] = (state->pc + 2) & 0b11111111;
            state->sp -= 2;
            state->pc = (opcode[2] << 8) | opcode[1];

            break;

        // ACI Add Immediate To Accumulator With Carry
        // Description: The byte of immediate data is added to \
           the contents of the accumulator plus the contents of the \
           carry bit.
        // ACI
        case 0xce:
            uint16_t res = (uint16_t) state->a + (uint16_t) opcode[1] + state->cc.cy;

            // zero flag
            if ((res & 0b11111111) == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            if (res > 0b11111111)
                state->cc.cy = 1;
            else
                state->cc.cy = 0;

            state->cc.p = parity(res & 0b11111111);

            state->a = res & 0b11111111;            

            state->pc += 1;

            break;

        case 0xcf: UnimplementedInstruction(state); break;

        // RNC Return If No Carry
        // Description: If the carry bit is zero, a return operation \
           is performed.
        // RNC
        case 0xd0:
            if (!(state->cc.cy)){
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
                state->sp += 2;
            }
            break;            

        case 0xd1: UnimplementedInstruction(state); break;

        // JNC Jump If No Carry
        // Description: If the Carry bit is zero, program execu- \
           tion continues at the memory address adr.
        // JNC
        case 0xd2: 
            if (!(state->cc.cy))
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc += 2;

            break;

        case 0xd3: UnimplementedInstruction(state); break;

        // CNC Call If No Carry
        // Description: If the Carry bit is zero, a call operation is \
           performed to subroutine sub.
        // CNC
        case 0xd4: 
            if (!(state->cc.cy)){
                state->memory[state->sp-1] = ((state->pc+2) >> 8) & 0b11111111;
                state->memory[state->sp-2] = (state->pc+2) & 0b11111111;
                state->sp -= 2;
                state->pc = (opcode[2] << 8) | opcode[1];
            }
            else
                state->pc += 2;
            break;

        case 0xd5: UnimplementedInstruction(state); break;

        // SUI Subtract Immediate From Accumulator
        // Description: The byte of immediate data is subtracted \
           from the contents of the accumulator using two's comple- \
           ment arithmetic. \
           Since this is a subtraction operation, the carry bit is \
           set, indicati ng a borrow, if there is no carry out of the high- \
           order bit position, and reset if there is a carry out.
        // 
        case 0xd6: 
            uint16_t res = (uint16_t) state->a - (uint16_t) opcode[1];

            // zero flag
            if ((res & 0b11111111) == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            if (res > 0b11111111)
                state->cc.cy = 0;
            else
                state->cc.cy = 1;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            state->a = res & 0b11111111;            

            state->pc += 1;

            break;

        case 0xd7: UnimplementedInstruction(state); break;

        // RC Return If Carry
        // Description: If the Carry bit is one, a return operation \
           is performed.
        // RC
        case 0xd8:
            if (state->cc.cy){
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
                state->sp += 2;
            }

            break;

        case 0xd9: UnimplementedInstruction(state); break;

        // JC Jump If Carry
        // Description: If the Carry bit is one, program execu- \
           tion conti nues at the memory address adr.
        // JC
        case 0xda:
            if (state->cc.cy)
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc += 2;
            
            break;

        case 0xdb: UnimplementedInstruction(state); break;
        // CC Call If Carry
        // Description: If the Carry bit is one, a call operation is \
           performed to subroutine sub.
        // CC
        case 0xdc:
            if (state->cc.cy){
                state->memory[state->sp-1] = ((state->pc+2) >> 8) & 0b11111111;
                state->memory[state->sp-2] = (state->pc+2) & 0b11111111;
                state->sp -= 2;
                state->pc = ((opcode[2]) << 8) | opcode[1];
            }
            else
                state->pc += 2;
            break;

        case 0xdd: UnimplementedInstruction(state); break;

        // SBI Subtract Immediate from Accumulator \
           With Borrow
        // Description: The Carry bit is internally added to the \
           byte of immediate data. This value is then subtracted from \
           the accumulator using two's complement arithmetic.
        // SBI
        case 0xde: 
            uint16_t res = (uint16_t) state->a - ((uint16_t) opcode[1] + state->cc.cy);

            // zero flag
            if ((res & 0b11111111) == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            // carry flag
            if (res > 0b11111111)
                state->cc.cy = 0;
            else
                state->cc.cy = 1;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            state->a = res & 0b11111111;            

            state->pc += 1;

            break;

        case 0xdf: UnimplementedInstruction(state); break;

        // RPO Return If Parity Odd
        // Description: If the Parity bit is zero (indicating odd \
           parity), a return operation is performed.
        // RPO
        case 0xe0:
            if (!(state->cc.p)){
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
                state->sp += 2;
            }
            break;

        case 0xe1: UnimplementedInstruction(state); break;

        // JPO Jump If Parity Odd
        // Description: If the Parity bit is zero (indicating a re- \
           sult with odd parity), program execution conti nues at the \
           memory address adr.
        // JPO
        case 0xe2:
            if (!(state->cc.p))
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc += 2;
            
            break;

        case 0xe3: UnimplementedInstruction(state); break;

        // CPO Call If Parity Odd
        // Description: If the Parity bit is zero (indicating odd \
           parity), a call operation is performed to subroutine sub.
        // CPO
        case 0xe4:
            if (!(state->cc.p)){
                state->memory[state->sp-1] = ((state->pc+2) >> 8) & 0b11111111;
                state->memory[state->sp-2] = (state->pc+2) & 0b11111111;
                state->sp -= 2;
                state->pc = ((opcode[2]) << 8) | opcode[1];
            }
            else
                state->pc += 2;
            
            break;

        case 0xe5: UnimplementedInstruction(state); break;

        // ANI And Immediate With Accumulator
        // Description: The byte of immediate data is logically \
           ANDed with the contents of the accumulator. The Carry bit \
           is reset to zero.
        // ANI
        case 0xe6: 
            state->a = state->a & opcode[1];

            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            //carry bit
            state->cc.z = 0;

            // parity flag
            state->cc.p = parity(state->a);

            state->pc += 1;

            break;

        case 0xe7: UnimplementedInstruction(state); break;

        // RPE Return If Parity Even
        // Description: If the Parity bit is one (indicating even \
           parity), a return operation is performed.
        // RPE
        case 0xe8:
            if (state->cc.p){
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
                state->sp += 2;
            }
            break;

        // PCHL Load Program Counter
        // Description: The contents of the H register replace the \
           most significant 8 bits of the program counter, and the con- \
           tents of the L register replace the least significant 8 bits of \
           the program counter. This causes program execution to con- \
           tinue at the address contained in the Hand L registers.
        // PCHL
        case 0xe9:
            state->pc = (state->h << 8) | state->l;

            break;
        
        // JPE Jump If Parity Even
        // Description: If the parity bit is one (indicating a result \
           with even parity), program execution continues at the mem- \
           ory address adr.
        // JPE
        case 0xea: 
            if (state->cc.p)
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc += 2;

            break;

        case 0xeb: UnimplementedInstruction(state); break;

        // CPE Call If Parity Even
        // Description: If the Parity bit is one (indicating even \
           parity), a call operation is performed to subroutine sub.
        // CPE
        case 0xec:
            if (state->cc.p){
                state->memory[state->sp-1] = ((state->pc+2) >> 8) & 0b11111111;
                state->memory[state->sp-2] = (state->pc+2) & 0b11111111;
                state->sp -= 2;
                state->pc = ((opcode[2]) << 8) | opcode[1];
            }
            else
                state->pc += 2;
            
            break;

        case 0xed: UnimplementedInstruction(state); break;

        // XRI Exclusive-Or Immediate With Accumulator
        // Description: The byte of immediate data is EXCLU- \
           SIV E-ORed with the contents of the accumulator. The carry \
           bit is set to zero.
        // XRI
        case 0xee: 
            state->a = state->a ^ opcode[1];

            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            //carry bit
            state->cc.z = 0;

            // parity flag
            state->cc.p = parity(state->a);

            state->pc += 1;

            break;

        case 0xef: UnimplementedInstruction(state); break;

        // RP Return If Plus
        // Description: If the Sign bit is zero (indicating a posi- \
           tive result). a return operation is performed.
        // RP
        case 0xf0: 
            if (!(state->cc.s)){
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
                state->sp += 2;
            }
            break;

        case 0xf1: UnimplementedInstruction(state); break;

        // JP Jump If Positive
        // Description: If the sign bit is zero, (indicating a posi- \
           tive result), program execution continues at the memory \
           address adr.
        // JP
        case 0xf2:
            if (!(state->cc.s))
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc += 2;

            break;

        case 0xf3: UnimplementedInstruction(state); break;

        // CP Call If Plus
        // Description: If the Sign bit is zero (indicating a posi- \
           tive result), a call operation is performed to subroutine sub.
        // CP
        case 0xf4:
            if (!(state->cc.s)){
                state->memory[state->sp-1] = ((state->pc+2) >> 8) & 0b11111111;
                state->memory[state->sp-2] = (state->pc+2) & 0b11111111;
                state->sp -= 2;
                state->pc = ((opcode[2]) << 8) | opcode[1];
            }
            else
                state->pc += 2;

            break;

        case 0xf5: UnimplementedInstruction(state); break;

        // ORI Or Immediate With Accumulator
        // Description: The byte of immediate data is logically \
           ORed with the contents of the accumulator
        // ORI
        case 0xf6:
            state->a = state->a | opcode[1];

            // zero flag
            if (state->a == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (state->a & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;
            
            //carry bit
            state->cc.z = 0;

            // parity flag
            state->cc.p = parity(state->a);

            state->pc += 1;

            break;

        case 0xf7: UnimplementedInstruction(state); break;

        // RM Return If Minus
        // Description: If the Sign bit is one (indicating a minus \
           result), a return operation is performed.
        // RM
        case 0xf8:
            if (state->cc.s){
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
                state->sp += 2;
            }

            break;

        case 0xf9: UnimplementedInstruction(state); break;

        // JM Jump If Minus
        // Description: If the Sign bit is one (indicating a nega- \
           tive result), program execution continues at the memory \
           address adr.
        // JM
        case 0xfa:
            if (state->cc.s)
                state->pc = (opcode[2] << 8) | opcode[1];
            else
                state->pc += 2;

            break;

        case 0xfb: UnimplementedInstruction(state); break;

        // CM Call If Minus
        // Description: If the Sign bit is one (indicating a minus \
           result), a call operation is performed to subrouti ne sub.
        // CM
        case 0xfc: 
            if (state->cc.s){
                state->memory[state->sp-1] = ((state->pc+2) >> 8) & 0b11111111;
                state->memory[state->sp-2] = (state->pc+2) & 0b11111111;
                state->sp -= 2;
                state->pc = ((opcode[2]) << 8) | opcode[1];
            }
            else
                state->pc += 2;
            
            break;

        case 0xfd: UnimplementedInstruction(state); break;

        // CPI Compare Immediate With Accumulator
        // Description: The byte of immediate data is compared \
           to the contents of the accumulator.
        // CPI
        case 0xfe:
            uint16_t res = (uint16_t) state->a - (uint16_t) opcode[1];

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
            state->cc.cy = (state->a <= opcode[1]) ? 0 : 1;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            state->pc += 1;

            break;

        case 0xff: UnimplementedInstruction(state); break;
    }
    state->pc++;


}