#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "processor.h"

// if the instruction is not implemented yet
void unimplemented_instruction(emu8080 *state)
{
    printf("error: unimplemented instruction %x\n", state->memory[state->pc]);
    exit(1);
}

void printConditionsEmu(emu8080 *emu, unsigned char *buf){
    printf("EMU:\n");
    sprintf(buf, "REGISTERS\n");
    char temp[50];

    sprintf(temp, "reg a: $%04x\n", emu->a);  
    strcat(buf, temp);
    sprintf(temp, "reg b: $%04x\n", emu->b);
    strcat(buf, temp);
    sprintf(temp, "reg c: $%04x\n", emu->c);
    strcat(buf, temp);
    sprintf(temp, "reg d: $%04x\n", emu->d);
    strcat(buf, temp);
    sprintf(temp, "reg e: $%04x\n", emu->e);
    strcat(buf, temp);
    strcat(buf, temp);
    sprintf(temp, "reg h: $%04x\n", emu->h);
    strcat(buf, temp);
    sprintf(temp, "reg l: $%04x\n", emu->l);
    strcat(buf, temp);
    sprintf(temp, "CONDITION FLAGS\n");
    strcat(buf, temp);
    sprintf(temp, "zf: $%04x\n", emu->cc.z);
    strcat(buf, temp);
    sprintf(temp, "sf: $%04x\n", emu->cc.s);
    strcat(buf, temp);
    sprintf(temp, "cf: $%04x\n", emu->cc.cy);
    strcat(buf, temp);
    sprintf(temp, "pf: $%04x\n", emu->cc.p);
    strcat(buf, temp);
    sprintf(temp, "POINTERS\n");
    strcat(buf, temp);
    sprintf(temp, "sp: $%04x\n", emu->sp);
    strcat(buf, temp);
    sprintf(temp, "pc: $%04x\n", emu->pc);
    strcat(buf, temp);
    
    printf("REGISTERS\n");
    printf("reg a: $%04x\n", emu->a);  
    printf("reg b: $%04x\n", emu->b);
    printf("reg c: $%04x\n", emu->c);
    printf("reg d: $%04x\n", emu->d);
    printf("reg e: $%04x\n", emu->e);
    printf("reg h: $%04x\n", emu->h);
    printf("reg l: $%04x\n", emu->l);
    printf("CONDITION FLAGS\n");
    printf("zf: $%04x\n", emu->cc.z);
    printf("sf: $%04x\n", emu->cc.s);
    printf("cf: $%04x\n", emu->cc.cy);
    printf("pf: $%04x\n", emu->cc.p);
    printf("POINTERS\n");
    printf("sp: $%04x\n", emu->sp);
    printf("pc: $%04x\n", emu->pc);
    puts("");
}

// handles output from the game; just printing here for now
void redirect_output(uint8_t byte, uint8_t port)
{
    printf("Output to port %d: %02X\n", port, byte);
}

// determines parity of a number
int parityEmu(int x, int size)
{
    int i;
    int p = 0;
    x = (x & ((1 << size) - 1));
    for (i = 0; i < size; i++)
    {
        if (x & 0x1)
        {
            p++;
        }
        x = x >> 1;
    }
    return (0 == (p & 0x1));
}

// sets flags after logical instruction on A register
void logic_flags_A(emu8080 *state)
{
    state->cc.cy = state->cc.ac = 0;
    state->cc.z = (state->a == 0);
    state->cc.s = (0x80 == (state->a & 0x80));
    state->cc.p = parityEmu(state->a, 8);
}

// set flags after arithmetic function on A register.
void arithmetic_flags_A(emu8080 *state, int answer)
{
    state->cc.cy = (answer > 0xff);
    state->cc.z = ((answer & 0xff) == 0);
    state->cc.s = ((answer & 0x80) != 0);
    state->cc.p = parityEmu(answer & 0xff, 8);
}


    int Emulate8080Op(State8080 *state){
    unsigned char *opcode = &state->memory[state->pc];
    Disassemble8080Op(state->memory, state->pc);
    state->pc += 1;

    switch (*opcode)
    {
    case 0x00:
        break;
    case 0x01:  // LXI B,D16
        state->c = opcode[1];
        state->b = opcode[2];
        state->pc += 2;
        break;
    case 0x02:  // STAX B
    {
        uint16_t offset = (state->b << 8) | state->c;
        state->memory[offset] = state->a;
    }
        break;
    case 0x03:  // INX B
    {
        uint16_t bc = (state->b << 8) | (state->c);
        bc += 1;
        state->b = (bc >> 8) & 0xff;
        state->c = bc & 0xff;
    }
    break;
        break;
    case 0x04:  // INR B
    {
        int8_t result = state->b + 1;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.ac = 0;
        state->b = result;
    }
        break;
    case 0x05: // 	DCR B
    {
        uint8_t result = state->b - 1;
        state->cc.z = (0 == result);
        state->cc.s = (0x80 == (result & 0x80));
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->b = result;
    }
        break;
    case 0x06:  // MVI B,D8
        state->b = opcode[1];
        state->pc += 1;
        break;
    case 0x07:  // RLC
    {
        uint8_t x = state->a;
        state->a = ((x & 0x80) >> 7) | x << 1;
        state->cc.cy = (x & 0x80) != 0;
    }
        break;
    case 0x08: 
        // Undocumented NOP
        break;
    case 0x09: // DAD B
    {
        uint32_t hl = (state->h << 8) | state->l;
        uint32_t bc = (state->b << 8) | state->c;
        uint32_t answer = hl + bc;
        state->h = (answer & 0xff00) >> 8;
        state->l = answer & 0xff;
        state->cc.cy = ((answer & 0xffff0000) != 0);
    }
        break;
    case 0x0a:  // LDAX B
    {
        uint16_t offset = (state->b << 8) | state->c;
        state->a = state->memory[offset];
    }
        break;
    case 0x0b:  // DCX B
    {
        uint16_t bc = (state->b << 8) | (state->c);
        bc -= 1;
        state->b = (bc >> 8) & 0xff;
        state->c = bc & 0xff;
    }
        break;
    case 0x0c:  // INR C
    {
        int8_t result = state->c + 1;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.ac = 0;
        state->c = result;
    }
        break;
    case 0x0d: //   DCR C
    {
        uint8_t result = state->c - 1;
        state->cc.z = (0 == result);
        state->cc.s = (0x80 == (result & 0x80));
        state->cc.p = parity(result, 8);
        state->c = result;
    }
        break;
    case 0x0e: //	MVI C,D8
        state->c = opcode[1];
        state->pc += 1;
        break;
    case 0x0f:  // RRC
        {
            uint8_t x = state->a;
            state->a = (x & 1) << 7 | (x >> 1);
            state->cc.cy = (1 == (x & 1));
        }
        break;
    case 0x10:
        // Undocumented NOP
        break;

    case 0x11: // LXI D,D16
        state->d = opcode[2];
        state->e = opcode[1];
        state->pc += 2;
        break;
    case 0x12:  // STAX D
    {
        uint16_t offset = (state->d << 8) | state->e;
        state->memory[offset] = state->a;
    }
        break;
    case 0x13: // 	INX D
    {
        uint16_t de = (state->d << 8) | (state->e);
        de += 1;
        state->d = (de >> 8) & 0xff;
        state->e = de & 0xff;
    }
        break;
    case 0x14:  // INR D
    {
        int8_t result = state->d + 1;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.ac = 0;
        state->d = result;
    }
        break;
    case 0x15:  // DCR D
    {
        int8_t result = state->d - 1;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.ac = 0;
        state->d = result;
    }
        break;
    case 0x16:  // MVI D
        state->d = opcode[1];
        state->pc += 1;
        break;
    case 0x17:  // RAL
    {
        uint8_t x = state->a;
        state->a = state->cc.cy | x << 1;
        state->cc.cy = (x & 0x80) != 0;
    }
        break;
    case 0x18:
        // Undocumented NOP
        break;
    case 0x19: //   DAD D
    {
        uint32_t hl = (state->h << 8) | state->l;
        uint32_t de = (state->d << 8) | state->e;
        uint32_t answer = hl + de;
        state->h = (answer & 0xff00) >> 8;
        state->l = answer & 0xff;
        state->cc.cy = ((answer & 0xffff0000) != 0);
    }
        break;
    case 0x1a: // 	LDAX D
    {
        uint16_t offset = (state->d << 8) | state->e;
        state->a = state->memory[offset];
    }
        break;
    case 0x1b:  // DCX D
    {
        uint16_t de = (state->d << 8) | (state->e);
        de -= 1;
        state->d = (de >> 8) & 0xff;
        state->e = de & 0xff;
    }
        break;
    case 0x1c:  // INR E
    {
        int8_t result = state->e + 1;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.ac = 0;
        state->e = result;
    }
        break;
    case 0x1d:  // DCR E
    {
        int8_t result = state->e - 1;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.ac = 0;
        state->e = result;
    }
        break;
    case 0x1e:  // MVI E
        state->e = opcode[1];
        state->pc += 1;
        break;
    case 0x1f:  // RAR
    {
        uint8_t x = state->a;
        state->a = (x & 0x80) | x >> 1;
        state->cc.cy = (x & 0x01) != 0;
    }
        break;

    case 0x20:  
        // Undocumented NOP
        break;
    case 0x21: // LXI H,D16
        state->h = opcode[2];
        state->l = opcode[1];
        state->pc += 2;
        break;
    case 0x22:  // SHLD adr
    {
        uint16_t offset = (opcode[2] << 8) | opcode[1];
        state->memory[offset] = state->l;
        state->memory[offset + 1] = state->h;
        state->pc += 2;
    }
        break;
    case 0x23: // INX H
        {
            uint16_t hl = (state->h << 8) | state->l;
            hl += 1;
            state->h = (hl >> 8) & 0xff;
            state->l = hl & 0xff;
        }
        break;
    case 0x24:  // INR H
    {
        int8_t result = state->h + 1;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.ac = 0;
        state->h = result;
    }
        break;
    case 0x25:  // DCR H
    {
        int8_t result = state->h - 1;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.ac = 0;
        state->h = result;
    }
        break;
    case 0x26:  // 	MVI H,D8
        state->h = opcode[1];
        state->pc += 1;
        break;
    case 0x27:  // DAA - special
        UnimplementedInstruction(state);
        // Decimal Adjust Accumulator & AC (auxillary carry) flag not implemented
        // Tests from cpudiag are skipped
        break;
    case 0x28:
        // Undocumented NOP
        break;
    case 0x29: // DAD H
        {
            uint32_t hl = (state->h << 8) | (state->l);
            uint32_t answer = hl + hl;
            state->h = (answer & 0xff00) >> 8;
            state->l = answer & 0xff;
            state->cc.cy = ((answer & 0xffff0000) != 0);
        }
        break;
    case 0x2a:  // LHLD adr
    {
        uint16_t offset = (opcode[2] << 8) | opcode[1];
        state->l = state->memory[offset];
        state->h = state->memory[offset+1];
        state->pc += 2;
    }
        break;
    case 0x2b:  // DCX H
    {
        uint16_t hl = (state->h << 8) | state->l;
        hl -= 1;
        state->h = (hl >> 8) & 0xff;
        state->l = hl & 0xff;
    }
        break;
    case 0x2c:  // INR L
    {
        int8_t result = state->l + 1;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.ac = 0;
        state->l = result;
    }
        break;
    case 0x2d:  // DCR L
    {
        int8_t result = state->l - 1;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.ac = 0;
        state->l = result;
    }
        break;
    case 0x2e:  // MVI L, D8
        state->l = opcode[1];
        state->pc += 1;
        break;
    case 0x2f:  // CMA
        state->a = ~(state->a);
        break;
    case 0x30:
        // Undocumented NOP
        break;
    case 0x31:  // LXI SP, D16
        state->sp = (opcode[2] << 8) | opcode[1];
        state->pc += 2;
        break;
    case 0x32:  // STA adr
    {
        uint16_t offset = (opcode[2] << 8) | (opcode[1]);
        state->memory[offset] = state->a;
        state->pc += 2;
    }
        break;
    case 0x33:  // INX SP
        state->sp += 1;
        break;
    case 0x34:  // INR M
    {
        int16_t offset = (state->h) << 8 | state->l;
        int8_t result = state->memory[offset] + 1;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.ac = 0;
        state->memory[offset] = result;
    }
        break;
    case 0x35:  // DCR M
    {
        int16_t offset = (state->h) << 8 | state->l;
        int8_t result = state->memory[offset] - 1;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.ac = 0;
        state->memory[offset] = result;
    }
        break;
    case 0x36: // 	MVI M,D8
        {
            uint16_t offset = (state->h << 8) | (state->l);
            state->memory[offset] = opcode[1];
            state->pc += 1;
        }
        break;
    case 0x37:  // STC
        state->cc.cy = 1;
        break;
    case 0x38:
        // Undocumented NOP
        break;
    case 0x39:  // DAD SP
    {
        uint32_t hl = (state->h << 8) | state->l;
        uint32_t answer = hl + state->sp;
        state->h = (answer & 0xff00) >> 8;
        state->l = answer & 0xff;
        state->cc.cy = ((answer & 0xffff0000) != 0);
    }
        break;
    case 0x3a:  // LDA adr
    {
        uint16_t offset = (opcode[2] << 8) | (opcode[1]);
        state->a = state->memory[offset];
        state->pc += 2;
    }
        break;
    case 0x3b:  // DCX SP
        state->sp -= 1;
        break;
    case 0x3c:  // INR A
    {
        int8_t result = state->a + 1;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0x3d:  // DCR A
    {
        int8_t result = state->a - 1;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0x3e:  // MVI A,D8
        state->a = opcode[1];
        state->pc += 1;
        break;
    case 0x3f:  // CMC
        state->cc.cy = !state->cc.cy;
        break;

    case 0x40:  // MOV B,B
        state->b = state->b;
        break;
    case 0x41:  // MOV B,C
        state->b = state->c;
        break;
    case 0x42:  // MOV B,D
        state->b = state->d;
        break;
    case 0x43:  // MOV B,E
        state->b = state->e;
        break;
    case 0x44:  // MOV B,H
        state->b = state->h;
        break;
    case 0x45:  // MOV B,L
        state->b = state->l;
        break;
    case 0x46:  // MOV B,M
    {
        uint16_t offset = (state->h << 8) | (state->l);
        state->b = state->memory[offset];
    }
        break;
    case 0x47:  // MOV B,A
        state->b = state->a;
        break;
    case 0x48:  // MOV C,B
        state->c = state->b;
        break;
    case 0x49:  // MOV C,C
        state->c = state->c;
        break;
    case 0x4a: // MOV C,D
        state->c = state->d;
        break;
    case 0x4b:  // MOV C,E
        state->c = state->e;
        break;
    case 0x4c:  // MOV C,H
        state->c = state->h;
        break;
    case 0x4d:  // MOV C,L
        state->c = state->l;
        break;
    case 0x4e:  // MOV C,M
    {
        uint16_t offset = (state->h << 8) | (state->l);
        state->c = state->memory[offset];
    }
        break;
    case 0x4f:  // MOV C,A
        state->c = state->a;
        break;

    case 0x50:  // MOV D,B
        state->d = state->b;
        break;
    case 0x51:  // MOV D,C
        state->d = state->c;
        break;
    case 0x52:  // MOV D,D
        state->d = state->d;
        break;
    case 0x53:  // MOV D,E
        state->d = state->e;
        break;
    case 0x54:  // MOV D,H
        state->d = state->h;
        break;
    case 0x55:  // MOV D,L
        state->d = state->l;
        break;
    case 0x56: // MOV D,M
    {
        uint16_t offset = (state->h << 8) | (state->l);
        state->d = state->memory[offset];
    }
        break;
    case 0x57:  // MOV D,A
        state->d = state->a;
        break;
    case 0x58:  // MOV E,B
        state->e = state->b;
        break;
    case 0x59:  // MOV E,C
        state->e = state->c;
        break;
    case 0x5a:  // MOV E,D
        state->e = state->d;
        break;
    case 0x5b:  // MOV E,E
        state->e = state->e;
        break;
    case 0x5c:  // MOV E,H
        state->e = state->h;
        break;
    case 0x5d:  // MOV E,L
        state->e = state->l;
        break;
    case 0x5e: // MOV E,M
    {
        uint16_t offset = (state->h << 8) | state->l;
        state->e = state->memory[offset];
    }
        break;
    case 0x5f:  // MOV E,A
        state->e = state->a;
        break;
    
    case 0x60:  // MOV H,B
        state->h = state->b;
        break;
    case 0x61:  // MOV H,C
        state->h = state->c;
        break;
    case 0x62:  // MOV H,D
        state->h = state->d;
        break;
    case 0x63:  // MOV H,E
        state->h = state->e;
        break;
    case 0x64:  // MOV H,H
        state->h = state->h;
        break;
    case 0x65:  // MOV H,L
        state->h = state->l;
        break;
    case 0x66:  // MOV H,M
    {
        uint16_t offset = (state->h << 8) | state->l;
        state->h = state->memory[offset];
    }
        break;
    case 0x67:  // MOV H,A
        state->h = state->a;
        break;
    case 0x68:  // MOV L,B
        state->l = state->b;
        break;
    case 0x69:  // MOV L,C
        state->l = state->c;
        break;
    case 0x6a:  // MOV L,D
        state->l = state->d;
        break;
    case 0x6b:  // MOV L,E
        state->l = state->e;
        break;
    case 0x6c:  // MOV L,H
        state->l = state->h;
        break;
    case 0x6d:  // MOV L,L
        state->l = state->l;
        break;
    case 0x6e:  // MOV L,M
    {
        uint16_t offset = (state->h << 8) | state->l;
        state->l = state->memory[offset];
    }
        break;
    case 0x6f: // MOV L,A
        state->l = state->a;
        break;
    
    case 0x70:  // MOV M,B
    {
        uint16_t offset = (state->h << 8) | state->l;
        state->memory[offset] = state->b;
    }
        break;
    case 0x71:  // MOV M,C
    {
        uint16_t offset = (state->h << 8) | state->l;
        state->memory[offset] = state->c;
    }
        break;
    case 0x72:  // MOV M,D
    {
        uint16_t offset = (state->h << 8) | state->l;
        state->memory[offset] = state->d;
    }
        break;
    case 0x73:  // MOV M,E
    {
        uint16_t offset = (state->h << 8) | state->l;
        state->memory[offset] = state->e;
    }
        break;
    case 0x74:  // MOV M,H
    {
        uint16_t offset = (state->h << 8) | state->l;
        state->memory[offset] = state->h;
    }
        break;
    case 0x75:  // MOV M,L
    {
        uint16_t offset = (state->h << 8) | state->l;
        state->memory[offset] = state->l;
    }
        break;
    case 0x76:  // HLT
        exit(0); // Exit on HALT instruction
        break;
    case 0x77: // 	MOV M,A
    {
        uint16_t offset = (state->h << 8) | (state->l);
        state->memory[offset] = state->a;
    }
        break;
    case 0x78:  // MOV A,B
        state->a = state->b;
        break;
    case 0x79:  // MOV A,C
        state->a = state->c;
        break;
    case 0x7a: // MOV A,D
        state->a = state->d;
        break;
    case 0x7b: // MOV A,E
        state->a = state->e;
        break;
    case 0x7c:  // MOV A,H
        state->a = state->h;
        break;
    case 0x7d:  // MOV A,L
        state->a = state->l;
        break;
    case 0x7e: // MOV A,M
    {
        uint16_t offset = (state->h << 8) | state->l;
        state->a = state->memory[offset];
    }
        break;
    case 0x7f:  // MOV A,A
        state->a = state->a;
        break;
    
    case 0x80:  // ADD B
    {        
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->b;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x81:  // ADD C
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->c;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x82:  // ADD D
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->d;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x83:  // ADD E
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->e;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x84:  // ADD H
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->h;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x85:  // ADD L
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->l;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x86:  // ADD M
    {
        uint16_t offset = (state->h) << 8 | state->l;
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->memory[offset];
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x87:  // ADD A
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->a;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x88:  // ADC B
    {
        uint16_t result = (uint16_t)state->a + (uint16_t)state->b + (uint16_t)state->cc.cy;
        state->cc.z = ((result & 0xff) == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = (result > 0xff);
        state->cc.ac = 0;
        state->a = result & 0xff;
    }
        break;
    case 0x89:  // ADC C
    {
        uint16_t result = (uint16_t)state->a + (uint16_t)state->c + (uint16_t)state->cc.cy;
        state->cc.z = ((result & 0xff) == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = (result > 0xff);
        state->cc.ac = 0;
        state->a = result & 0xff;
    }
        break;
    case 0x8a:  // ADC D
    {
        uint16_t result = (uint16_t)state->a + (uint16_t)state->d + (uint16_t)state->cc.cy;
        state->cc.z = ((result & 0xff) == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = (result > 0xff);
        state->cc.ac = 0;
        state->a = result & 0xff;
    }
        break;
    case 0x8b:  // ADC E
    {
        uint16_t result = (uint16_t)state->a + (uint16_t)state->e + (uint16_t)state->cc.cy;
        state->cc.z = ((result & 0xff) == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = (result > 0xff);
        state->cc.ac = 0;
        state->a = result & 0xff;
    }
        break;
    case 0x8c:  // ADC H
    {
        uint16_t result = (uint16_t)state->a + (uint16_t)state->h + (uint16_t)state->cc.cy;
        state->cc.z = ((result & 0xff) == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = (result > 0xff);
        state->cc.ac = 0;
        state->a = result & 0xff;
    }
        break;
    case 0x8d:  // ADC L
    {
        uint16_t result = (uint16_t)state->a + (uint16_t)state->l + (uint16_t)state->cc.cy;
        state->cc.z = ((result & 0xff) == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = (result > 0xff);
        state->cc.ac = 0;
        state->a = result & 0xff;
    }
        break;
    case 0x8e:  // ADC M
    {
        uint16_t offset = (state->h) << 8 | state->l;
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->memory[offset] + (uint16_t)state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x8f:  // ADC A
    {
        uint16_t result = (uint16_t)state->a + (uint16_t)state->a + (uint16_t)state->cc.cy;
        state->cc.z = ((result & 0xff) == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = (result > 0xff);
        state->cc.ac = 0;
        state->a = result & 0xff;
    }
        break;
    
    case 0x90:  // SUB B
    {
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->b;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x91:  // SUB C
    {
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->c;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x92:  // SUB D
    {
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->d;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x93:  // SUB E
    {
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->e;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x94:  // SUB H
    {
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->h;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x95:  // SUB L
    {
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->l;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x96:  // SUB M
    {
        uint16_t offset = (state->h) << 8 | state->l;
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->memory[offset];
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x97:  // SUB A
    {
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->a;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x98:  // SBB B
    {
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->b - (uint16_t)state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x99:  // SBB C
    {
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->c - (uint16_t)state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x9a:  // SBB D
    {
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->d - (uint16_t)state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x9b:  // SBB E
    {
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->e - (uint16_t)state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x9c:  // SBB H
    {
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->h - (uint16_t)state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x9d:  // SBB L
    {
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->l - (uint16_t)state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x9e:  // SBB M
    {
        uint16_t offset = (state->h) << 8 | state->l;
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->memory[offset] - (uint16_t)state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0x9f:  // SBB A
    {
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->a - (uint16_t)state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = parity(answer & 0xff, 8);
        state->a = answer & 0xff;
    }
        break;
    case 0xa0:  // ANA B
    {
        uint8_t result = state->a & state->b;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xa1:  // ANA C
    {
        uint8_t result = state->a & state->c;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xa2:  // ANA D
    {
        uint8_t result = state->a & state->d;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xa3:  // ANA E
    {
        uint8_t result = state->a & state->e;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xa4:  // ANA H
    {
        uint8_t result = state->a & state->h;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xa5:  // ANA L
    {
        uint8_t result = state->a & state->l;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xa6:  // ANA M
    {
        uint16_t offset = (state->h) << 8 | state->l;
        uint8_t result = state->a & state->memory[offset];
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xa7:  // ANA A
    {
        uint8_t result = state->a & state->a;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xa8:  // XRA B
    {
        uint8_t result = state->a ^ state->b;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xa9:  // XRA C
    {
        uint8_t result = state->a | state->c;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xaa:  // XRA D
    {
        uint8_t result = state->a | state->d;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xab:  // XRA E
    {
        uint8_t result = state->a | state->e;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xac:  // XRA H
    {
        uint8_t result = state->a | state->h;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xad:  // XRA L
    { 
        uint8_t result = state->a | state->l;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xae:  // XRA M
    {
        uint16_t offset = (state->h) << 8 | state->l;
        uint8_t result = state->a ^ state->memory[offset];
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xaf: // XRA A
    {
        uint8_t result = (state->a) ^ (state->a);
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;

    case 0xb0:  // ORA B
    {
        uint8_t result = state->a | state->b;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xb1:  // ORA C
    {
        uint8_t result = state->a | state->c;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xb2:  // ORA D
    {
        uint8_t result = state->a | state->d;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xb3:  // ORA E
    {
        uint8_t result = state->a | state->e;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xb4:  // ORA H
    {
        uint8_t result = state->a | state->h;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xb5:  // ORA L
    {
        uint8_t result = state->a | state->l;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xb6:  // ORA M
    {
        uint16_t offset = (state->h) << 8 | state->l;
        uint8_t result = state->a | state->memory[offset];
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xb7:  // ORA A
    {
        uint8_t result = state->a | state->a;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
    }
        break;
    case 0xb8:  // CMP B
    {
        uint8_t result = state->a - state->b;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.cy = (state->a < result);
    }
        break;
    case 0xb9:  // CMP C
    {
        uint8_t result = state->a - state->c;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.cy = (state->a < result);
    }
        break;
    case 0xba:  // CMP D
    {
        uint8_t result = state->a - state->d;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.cy = (state->a < result);
    }
        break;
    case 0xbb:  // CMP E
    {
        uint8_t result = state->a - state->e;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.cy = (state->a < result);
    }
        break;
    case 0xbc:  // CMP H
    {
        uint8_t result = state->a - state->h;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.cy = (state->a < result);
    }
        break;
    case 0xbd:  // CMP L
    {
        uint8_t result = state->a - state->l;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.cy = (state->a < result);
    }
        break;
    case 0xbe:  // CMP M
    {
        uint16_t offset = (state->h) << 8 | state->l;
        uint8_t result = state->a - state->memory[offset];
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.cy = (state->a < result);
    }
        break;
    case 0xbf:  // CMP A
    {
        uint8_t result = state->a - state->a;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.cy = (state->a < result);
    }
        break;

    case 0xc0:  // RNZ
        if (state->cc.z == 0)
        {
            uint16_t ret = (state->memory[state->sp + 1] << 8) | state->memory[state->sp];
            state->pc = ret;
            state->sp += 2;
        }
        break;
    case 0xc1: // 	POP B
        state->c = state->memory[state->sp];
        state->b = state->memory[state->sp + 1];
        state->sp += 2;
        break;
    case 0xc2: //	JNZ adr
        if(state->cc.z == 0)
            state->pc = (opcode[2] << 8) | (opcode[1]);
        else
            state->pc += 2;
        break;
    case 0xc3:  // JMP adr
        state->pc = (opcode[2] << 8) | opcode[1];
        break;
    case 0xc4:  // CNZ adr
        if (state->cc.z == 0){
            uint16_t ret = state->pc + 2;
            state->memory[state->sp - 1] = (ret >> 8) & 0xff; // shift 8 bits FIRST then mask to fit into memory
            state->memory[state->sp - 2] = (ret & 0xff);
            state->sp -= 2;
            state->pc = (opcode[2] << 8) | (opcode[1]);
        }
        else{
            state->pc += 2;
        }
        break;
    case 0xc5:
        state->memory[state->sp - 2] = state->c;
        state->memory[state->sp - 1] = state->b;
        state->sp -= 2;
        break;
    case 0xc6: // ADI D8
    {
        uint16_t result = (uint16_t) state->a + (uint16_t) opcode[1];
        state->cc.z = ((result & 0xff)== 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity((result & 0xff), 8);
        state->cc.cy = (result > 0xff);
        state->a = (uint8_t) result;
        state->pc += 1;
    }
        break;
    case 0xc7:  // RST 0
    {
        uint16_t ret = state->pc + 2;
        state->memory[state->sp - 1] = (ret >> 8) & 0xff;
        state->memory[state->sp - 2] = (ret & 0xff);
        state->sp -= 2;
        state->pc = 0x0;
    }
        break;
    case 0xc8:  // RZ
        if (state->cc.z == 1)
        {
            uint16_t ret = (state->memory[state->sp + 1] << 8) | state->memory[state->sp];
            state->pc = ret;
            state->sp += 2;
        }
        break;
    case 0xc9:  // RET
        {
            uint16_t ret = (state->memory[state->sp + 1] << 8) | state->memory[state->sp];
            state->pc = ret;
            state->sp += 2;
        }
        break;
    case 0xca:  // JZ adr
        if(state->cc.z == 1)
            state->pc = (opcode[2] << 8) | opcode[1];
        else
            state->pc += 2;
        
        break;
    case 0xcb:  // Undocumented JMP
        break;
    case 0xcc:  // CZ adr
        if (state->cc.z == 1){
            uint16_t ret = state->pc + 2;
            state->memory[state->sp - 1] = (ret >> 8) & 0xff; // shift 8 bits FIRST then mask to fit into memory
            state->memory[state->sp - 2] = (ret & 0xff);
            state->sp -= 2;
            state->pc = (opcode[2] << 8) | (opcode[1]);
        }
        else{
            state->pc += 2;
        }
        break;
    case 0xcd:  // CALL adr
    #ifdef FOR_CPUDIAG
        /* Code from Emulator101 to print cpudiag specific calls */
        if (5 == ((opcode[2] << 8) | opcode[1])){
            if (state->c == 9){
                uint16_t offset = (state->d << 8) | (state->e);
                char *str = &state->memory[offset + 3];
                printf("\t\t *****");
                while (*str != '$')
                    printf("%c", *str++);
                printf(" ***** \n");
                exit(0);
            }
            else if (state->c == 2){
                printf("print char routine called\n");
            }
        }
        else if (0 == ((opcode[2] << 8) | opcode[1])){
            exit(0);
        }
        else
    #endif
        {
            uint16_t ret = state->pc + 2;
            state->memory[state->sp - 1] = (ret >> 8) & 0xff; // shift 8 bits FIRST then mask to fit into memory
            state->memory[state->sp - 2] = (ret & 0xff);
            state->sp -= 2;
            state->pc = (opcode[2] << 8) | (opcode[1]);
        }
        break;
    case 0xce:  // ACI D8
    {
        uint16_t result = (uint16_t)state->a + (uint16_t)opcode[1] + (uint16_t)state->cc.cy;
        state->cc.z = ((result & 0xff) == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity((result & 0xff), 8);
        state->cc.cy = (result > 0xff);
        state->cc.ac = 0;
        state->a = (uint8_t)result;
        state->pc += 1;
    }
        break;
    case 0xcf:  // RST 1
    {
        uint16_t ret = state->pc + 2;
        state->memory[state->sp - 1] = (ret >> 8) & 0xff;
        state->memory[state->sp - 2] = (ret & 0xff);
        state->sp -= 2;
        state->pc = 0x8;
    }
        break;

    case 0xd0:  // RNC
        if (state->cc.cy == 0)
        {
            uint16_t ret = (state->memory[state->sp + 1] << 8) | state->memory[state->sp];
            state->pc = ret;
            state->sp += 2;
        }
        break;
    case 0xd1: //   POP D
        state->e = state->memory[state->sp];
        state->d = state->memory[state->sp + 1];
        state->sp += 2;
        break;
    case 0xd2:  // JNC adr
        if(state->cc.cy == 0)
            state->pc = (opcode[2] << 8) | opcode[1];
        else
            state->pc += 2;
        break;
    case 0xd3: //	OUT D8 - special
        state->pc += 1;
        break;
    case 0xd4:  // CNC adr
        if (state->cc.cy == 0)
        {
            uint16_t ret = state->pc + 2;
            state->memory[state->sp - 1] = (ret >> 8) & 0xff; // shift 8 bits FIRST then mask to fit into memory
            state->memory[state->sp - 2] = (ret & 0xff);
            state->sp -= 2;
            state->pc = (opcode[2] << 8) | (opcode[1]);
        }
        else
        {
            state->pc += 2;
        }
        break;
    case 0xd5: // 	PUSH D
        state->memory[state->sp - 2] = state->e;
        state->memory[state->sp - 1] = state->d;
        state->sp -= 2;
        break;
    case 0xd6:  // SUI D8
    {
        uint16_t result = (uint16_t)state->a - (uint16_t)opcode[1];
        state->cc.z = ((result & 0xff) == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = (result > 0xff);
        state->cc.ac = 0;
        state->a = result & 0xff;
        state->pc += 1;
    }
        break;
    case 0xd7:  // RST 2
    {
        uint16_t ret = state->pc + 2;
        state->memory[state->sp - 1] = (ret >> 8) & 0xff;
        state->memory[state->sp - 2] = (ret & 0xff);
        state->sp -= 2;
        state->pc = 0x10;
    }
        break;
    case 0xd8:  // RC
        if (state->cc.cy == 1)
        {
            uint16_t ret = (state->memory[state->sp + 1] << 8) | state->memory[state->sp];
            state->pc = ret;
            state->sp += 2;
        }
        break;
    case 0xd9:  // Undocumented RET
        break;
    case 0xda:  // JC adr
        if(state->cc.cy == 1)
            state->pc = (opcode[2] << 8) | opcode[1];
        else 
            state->pc += 2;
        break;
    case 0xdb:  // IN D8 - special
        state->pc += 1;
        break;
    case 0xdc:  // CC adr
        if(state->cc.cy == 1){
            uint16_t ret = state->pc + 2;
            state->memory[state->sp - 1] = (ret >> 8) & 0xff; // shift 8 bits FIRST then mask to fit into memory
            state->memory[state->sp - 2] = (ret & 0xff);
            state->sp -= 2;
            state->pc = (opcode[2] << 8) | (opcode[1]);
        }
        else{
            state->pc += 2;
        }
        break;
    case 0xdd: // Undocumented CALL
        break;
    case 0xde:  // SBI D8
    {
        uint16_t result = (uint16_t)state->a - (uint16_t)opcode[1] - (uint16_t)state->cc.cy;
        state->cc.z = ((result & 0xff) == 0);
        state->cc.s = ((result & 0x80) != 0);
        state->cc.p = parity(result, 8);
        state->cc.cy = (result > 0xff);
        state->cc.ac = 0;
        state->a = result & 0xff;
        state->pc += 1;
    }
        break;
    case 0xdf:  // RST 3
    {
        uint16_t ret = state->pc + 2;
        state->memory[state->sp - 1] = (ret >> 8) & 0xff;
        state->memory[state->sp - 2] = (ret & 0xff);
        state->sp -= 2;
        state->pc = 0x18;
    }
        break;

    case 0xe0:  // RPO
        if (state->cc.p == 0){
            uint16_t ret = (state->memory[state->sp + 1] << 8) | state->memory[state->sp];
            state->pc = ret;
            state->sp += 2;
        }
        break;
    case 0xe1: // POP H
        state->l = state->memory[state->sp];
        state->h = state->memory[state->sp + 1];
        state->sp += 2;
        break;
    case 0xe2:  // JPO
        if(state->cc.p == 0)
            state->pc = (opcode[2] << 8) | opcode[1];
        else
            state->pc += 2;
        break;
    case 0xe3:  // XTHL
    {
        uint8_t temp_h = state->h;
        uint8_t temp_l = state->l;
        state->h = state->memory[state->sp + 1];
        state->l = state->memory[state->sp];
        state->memory[state->sp + 1] = temp_h;
        state->memory[state->sp] = temp_l;
    }
        break;
    case 0xe4:  // CPO adr
        if (state->cc.p == 0){
            uint16_t ret = state->pc + 2;
            state->memory[state->sp - 1] = (ret >> 8) & 0xff; // shift 8 bits FIRST then mask to fit into memory
            state->memory[state->sp - 2] = (ret & 0xff);
            state->sp -= 2;
            state->pc = (opcode[2] << 8) | (opcode[1]);
        }
        else{
            state->pc += 2;
        }
        break;
    case 0xe5: // 	PUSH H
        state->memory[state->sp - 2] = state->l;
        state->memory[state->sp - 1] = state->h;
        state->sp -= 2;
        break;
    case 0xe6: // ANI D8
    {
        uint8_t result = state->a & opcode[1];
        state->a = result;
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->pc += 1;
    }
        break;
    case 0xe7:  // RST 4
    {
        uint16_t ret = state->pc + 2;
        state->memory[state->sp - 1] = (ret >> 8) & 0xff;
        state->memory[state->sp - 2] = (ret & 0xff);
        state->sp -= 2;
        state->pc = 0x20;
    }
        break;
    case 0xe8:  // RPE
        if(state->cc.p == 1){
            uint16_t ret = (state->memory[state->sp + 1] << 8) | state->memory[state->sp];
            state->pc = ret;
            state->sp += 2;
        }
        break;
    case 0xe9:  // PCHL
    {
        uint16_t offset = (state->h) << 8 | state->l;
        state->pc = offset;
    }
        break;
    case 0xea:  // JPE adr
        if(state->cc.p == 1)
            state->pc = (opcode[2] << 8) | opcode[1];
        else
            state->pc += 2;
        break;
    case 0xeb: //	XCHG
    {
        uint8_t temp = state->h;
        state->h = state->d;
        state->d = temp;

        temp = state->l;
        state->l = state->e;
        state->e = temp;
    }
        break;
    case 0xec:  // CPE adr
        if (state->cc.p == 1){
            uint16_t ret = state->pc + 2;
            state->memory[state->sp - 1] = (ret >> 8) & 0xff; // shift 8 bits FIRST then mask to fit into memory
            state->memory[state->sp - 2] = (ret & 0xff);
            state->sp -= 2;
            state->pc = (opcode[2] << 8) | (opcode[1]);
        }
        else{
            state->pc += 2;
        }
        break;
    case 0xed: // Undocumented CALL
        break;
    case 0xee:  // XRI D8
    {
        uint8_t result = state->a ^ opcode[1];
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
        state->pc += 1;
    }
        break;
    case 0xef:  // RST 5
    {
        uint16_t ret = state->pc + 2;
        state->memory[state->sp - 1] = (ret >> 8) & 0xff;
        state->memory[state->sp - 2] = (ret & 0xff);
        state->sp -= 2;
        state->pc = 0x28;
    }
        break;

    case 0xf0:  // RP
        if (state->cc.s == 0){
            uint16_t ret = (state->memory[state->sp + 1] << 8) | state->memory[state->sp];
            state->pc = ret;
            state->sp += 2;
        }
        break;
    case 0xf1:  // POP PSW
    {        
        uint8_t flags = state->memory[state->sp];
        state->cc.z = ((flags & 0x1) == 0x1);
        state->cc.s = ((flags & 0x2) == 0x2);
        state->cc.p = ((flags & 0x4) == 0x4);
        state->cc.cy = ((flags & 0x8) == 0x8);
        state->cc.ac = ((flags & 0x10) == 0x10);
        state->a = state->memory[state->sp + 1];
        state->sp += 2;
    }
        break;
    case 0xf2:  // JP adr
        if(state->cc.s == 0)
            state->pc = (opcode[2] << 8) | opcode[1];
        else
            state->pc += 2;
        break;
    case 0xf3:  // DI
        state->int_enable = 0;
        break;
    case 0xf4:  // CP adr
        if (state->cc.s == 0){
            uint16_t ret = state->pc + 2;
            state->memory[state->sp - 1] = (ret >> 8) & 0xff; // shift 8 bits FIRST then mask to fit into memory
            state->memory[state->sp - 2] = (ret & 0xff);
            state->sp -= 2;
            state->pc = (opcode[2] << 8) | (opcode[1]);
        }
        else{
            state->pc += 2;
        }
        break;
    case 0xf5: // 	PUSH PSW
    {
        state->memory[state->sp - 1] = state->a;
        uint8_t flags = (
            state->cc.z |
            state->cc.s << 1 | 
            state->cc.p << 2 |
            state->cc.cy << 3 | 
            state->cc.ac << 4
        );
        state->memory[state->sp - 2] = flags;
        state->sp -= 2;
    }
        break;
    case 0xf6:  // ORI D8
    {
        uint8_t result = state->a | opcode[1];
        state->cc.z = (result == 0);
        state->cc.s = ((result & 0x80) == 0x80);
        state->cc.p = parity(result, 8);
        state->cc.cy = 0;
        state->cc.ac = 0;
        state->a = result;
        state->pc += 1;
    }   
        break;
    case 0xf7:  // RST 6
    {
        uint16_t ret = state->pc + 2;
        state->memory[state->sp - 1] = (ret >> 8) & 0xff;
        state->memory[state->sp - 2] = (ret & 0xff);
        state->sp -= 2;
        state->pc = 0x30;
    }
        break;
    case 0xf8:  // RM
        if (state->cc.s == 1){
            uint16_t ret = (state->memory[state->sp + 1] << 8) | state->memory[state->sp];
            state->pc = ret;
            state->sp += 2;
        }
        break;
    case 0xf9:  // SPHL
    {
        uint16_t hl = (state->h << 8) | state->l;
        state->sp = hl;
    }
        break;
    case 0xfa:  // JM
        if (state->cc.s == 1)
            state->pc = (opcode[2] << 8) | opcode[1];
        else
            state->pc += 2;
        break;
    case 0xfb:  // EI
        state->int_enable = 1;
        break;
    case 0xfc:  // CM adr
        if (state->cc.s == 1){
            uint16_t ret = state->pc + 2;
            state->memory[state->sp - 1] = (ret >> 8) & 0xff; // shift 8 bits FIRST then mask to fit into memory
            state->memory[state->sp - 2] = (ret & 0xff);
            state->sp -= 2;
            state->pc = (opcode[2] << 8) | (opcode[1]);
        }
        else{
            state->pc += 2;
        }
        break;
    case 0xfd:  // Undocumented CALL
        break;
    case 0xfe: // 	CPI D8
    {
        uint8_t result = state->a - opcode[1];
        state->cc.z = (result == 0);
        state->cc.s = (0x80 == (result & 0x80));
        state->cc.p = parity(result, 8);
        state->cc.cy = (state->a < opcode[1]);
        state->pc += 1;
    }
        break;
    case 0xff:  // RST 7
    {
        uint16_t ret = state->pc + 2;
        state->memory[state->sp - 1] = (ret >> 8) & 0xff;
        state->memory[state->sp - 2] = (ret & 0xff);
        state->sp -= 2;
        state->pc = 0x38;
    }
        break;
    } // end of switch

// print the processor state condition codes (flags)
#ifdef DEBUG
    printf("\tC=%d,P=%d,S=%d,Z=%d\n", state->cc.cy, state->cc.p,
           state->cc.s, state->cc.z);
    // registers
    printf("\tA $%02x B $%02x C $%02x D $%02x E $%02x H $%02x L $%02x SP %04x PC %04x\n",
           state->a, state->b, state->c, state->d,
           state->e, state->h, state->l, state->sp, state->pc);
#endif

    return *cycles;
}