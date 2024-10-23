#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>


#include "./emugit/processor.h"
#include "../Disassembler/disDebug.h"

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

    // bit shifts
    uint8_t shiftLSB;
    uint8_t shiftMSB;
    uint8_t shift_offset;

} State8080;

bool compareRes(unsigned char *bufferEmu, unsigned char *bufferState){
    for (int i = 0; i < 198; i++){
        if (bufferEmu[i] != bufferState[i]){
            printf("there is a difference in conditions, exiting\n");
            printf("bufferEmu -1i1: %c%c%c, i: %d\n", bufferEmu[i-1], bufferEmu[i], bufferEmu[i+1], i);
            printf("bufferState -1i1: %c%c%c, i: %d\n", bufferState[i-1], bufferState[i], bufferState[i+1], i);
            exit(0);
        }
    }
}

void printConditionsState(State8080 *state, unsigned char *buf){
    printf("STATE:\n");
    sprintf(buf, "REGISTERS\n");
    char temp[50];

    sprintf(temp, "reg a: $%04x\n", state->a);  
    strcat(buf, temp);
    sprintf(temp, "reg b: $%04x\n", state->b);
    strcat(buf, temp);
    sprintf(temp, "reg c: $%04x\n", state->c);
    strcat(buf, temp);
    sprintf(temp, "reg d: $%04x\n", state->d);
    strcat(buf, temp);
    sprintf(temp, "reg e: $%04x\n", state->e);
    strcat(buf, temp);
    strcat(buf, temp);
    sprintf(temp, "reg h: $%04x\n", state->h);
    strcat(buf, temp);
    sprintf(temp, "reg l: $%04x\n", state->l);
    strcat(buf, temp);
    sprintf(temp, "CONDITION FLAGS\n");
    strcat(buf, temp);
    sprintf(temp, "zf: $%04x\n", state->cc.z);
    strcat(buf, temp);
    sprintf(temp, "sf: $%04x\n", state->cc.s);
    strcat(buf, temp);
    sprintf(temp, "cf: $%04x\n", state->cc.cy);
    strcat(buf, temp);
    sprintf(temp, "pf: $%04x\n", state->cc.p);
    strcat(buf, temp);
    sprintf(temp, "POINTERS\n");
    strcat(buf, temp);
    sprintf(temp, "sp: $%04x\n", state->sp);
    strcat(buf, temp);
    sprintf(temp, "pc: $%04x\n", state->pc);
    strcat(buf, temp);
        
    printf("REGISTERS\n");
    printf("reg a: $%04x\n", state->a);  
    printf("reg b: $%04x\n", state->b);
    printf("reg c: $%04x\n", state->c);
    printf("reg d: $%04x\n", state->d);
    printf("reg e: $%04x\n", state->e);
    printf("reg h: $%04x\n", state->h);
    printf("reg l: $%04x\n", state->l);
    printf("CONDITION FLAGS\n");
    printf("zf: $%04x\n", state->cc.z);
    printf("sf: $%04x\n", state->cc.s);
    printf("cf: $%04x\n", state->cc.cy);
    printf("pf: $%04x\n", state->cc.p);
    printf("POINTERS\n");
    printf("sp: $%04x\n", state->sp);
    printf("pc: $%04x\n", state->pc);
    puts("");
}


int parity(uint8_t b){
    int res = (b >> 7) & 0b1;

    for (int i = 6; i > -1; i--)
        res = res^((b >> i) & 0b1);

    return !res;
}

void render(State8080 *state, bool rendHalf, SDL_Renderer *renderer){
    int Px = 0, Py = 0, shift;
    // used to analyze individual bit
    int bit;
    
    // 256x224
    // stops at line 96 one px before center px of the screen
    if (rendHalf){
        for (int mem = 0x2000; mem <= 0x2b7b; mem++){
            for (shift = 0 ; shift < 8; shift++){
                //selects bit to be analyzed
                bit = (state->memory[mem] >> shift) & 0b1;

                if (bit)
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

                SDL_RenderDrawPoint(renderer, Px, Py);
                Px++;
                if (Px == 256){
                    Px = 0;
                    Py++;
                }
            }
        }
    }
    else{
        Px = 128;
        Py = 95;
        for (int mem = 0x2b7c; mem <= 0x3fff; mem++){
            for (shift = 0; shift < 8; shift++){
                //selects bit to be analyzed
                bit = (state->memory[mem] >> shift) & 0b1;

                if (bit)
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                else
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                
                SDL_RenderDrawPoint(renderer, Px, Py);
                Px++;
                if (Px == 256){
                    Px = 0;
                    Py++;       
                }
            }
        }
    }
    SDL_RenderPresent(renderer);
}

uint8_t MachineIN(State8080 *state, uint8_t port){
    uint8_t a = 0b00000000;
    
    switch(port){
        case 0:
            return 1;
        case 1:
            // bit 0 = CREDIT (1 if deposited); bit 2 = 1P start; bit 3 always 1
            a = 0b00001101;
            // config keyboard events
            SDL_Event e;
            if (SDL_PollEvent(&e)){
                if (e.type == SDL_KEYDOWN){
                    switch(e.key.keysym.sym){
                        case SDLK_LEFT:
                            a |= 0b00100000;
                            break;
                        case SDLK_RIGHT:
                            a |= 0b01000000;
                            break;
                        case SDLK_SPACE:
                            a |= 0b00010000;
                            break;
                    }
                }
                else if (e.type == SDL_KEYUP){
                    switch(e.key.keysym.sym){
                        case SDLK_LEFT:
                            a &= 0b11011111;
                            break;
                        case SDLK_RIGHT:
                            a &= 0b10111111;
                            break;
                        case SDLK_SPACE:
                            a &= 0b11101111;
                            break;
                    }
                }
            }
            break;

        case 2:
            return 0;

        case 3:{
            uint16_t v = (state->shiftMSB << 8) | state->shiftLSB;
            a = ((v >> (8-state->shift_offset)) & 0b11111111);
        }
        break;
    }
    return a;
}

void MachineOUT(State8080 *state, uint8_t port){
    switch (port){
        case 2:
            state->shift_offset = state->a & 0b111;
            break;
        
        case 4:
            state->shiftLSB = state->shiftMSB;
            state->shiftMSB = state->a;
            break;
    }
}

void GenerateInterrupt(State8080* state, int interrupt_num){
    state->memory[state->sp - 1] = ((state->pc+1) >> 8) & 0b11111111;
    state->memory[state->sp - 2] =  (state->pc+1) & 0b11111111;
    state->sp -= 2;
    
    state->pc = 8* interrupt_num;    
    state->pc--;    
}

// CPU clock rate = 2MHz; 1 clock per 500 ns; 1 frame per 16666667 ns; half frame per 8333334 ns
// time per frames rendered / CPU time per clock = (16666667 ns) / (500 ns) = 33334 cycles;  divider by 2 = 16667 cycles per half frame
void interProtocol(State8080 * state, bool *rendHalf, long long *curT, long long *lastInterrupt, struct timespec *start,SDL_Renderer *renderer, float *cycles){
    
    // if not enough time for interrupt, wait
    while(*curT - *lastInterrupt < 8333334){
        clock_gettime(CLOCK_MONOTONIC, start);
        *curT = ((*start).tv_sec * 1e9) + (*start).tv_nsec;
    }
    
    //only do an interrupt if they are enabled    
    if (state->int_enable){
        // render changes on screen
        render(state, *rendHalf, renderer);

        // different interrupts for different screen loads (upper or lower)
        if (*rendHalf)
            GenerateInterrupt(state, 1);
        else
            GenerateInterrupt(state, 2);

        // switch between upper and lower half of screen to be rendered
        *rendHalf = !(*rendHalf);
        // restart cycle count
        *cycles = 0;
    }    

    // Save the time we did this    
    clock_gettime(CLOCK_MONOTONIC, start);
    *lastInterrupt = ((*start).tv_sec * 1e9) + (*start).tv_nsec;
}

void UnimplementedInstruction(State8080 *state){
    state->pc--;
    printf("Error: instruction not implemented\n");
    exit(EXIT_FAILURE);
}

int Emulate8080Op(State8080 *state, float * cycles){
    unsigned char *opcode = &state->memory[state->pc];
    if (opcode == NULL){
        printf("opcode pointer is NULL\n");
        exit(1);
    }
    
    switch(*opcode){
        case 0x00: 
            (*cycles) += 4;
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
            (*cycles) += 10;
            break;
        
        // STAX Store accumulator
        // Description: The contents of the accumulator are \
           stored in the memory location addressed by registers B \
           and C, or by registers D and E.
        // STAX B
        case 0x02:
            state->memory[(state->b << 8) | state->c] = state->a; 
            (*cycles) += 7;
            break;
        
        // INX Increment Register Pair \
           Description: The 16-bit number held in the specified \
           register pair is incremented by one.
        //  INX BC
        case 0x03:
            uint16_t temp = (state->b << 8) | state->c;
            temp++;
            state->b = temp >> 8;
            state->c = temp & 0b11111111;
            (*cycles) += 5;
            break;

        // INR Increment Register or Memory
        // Description: The specified register or memory byte is \
           incremented by one.
        // INR B
        case 0x04:{
            uint16_t res = state->b + 1;

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

            state->b += 1;

            (*cycles) += 5;
            break;
        }

        // DCR Decrement Register or Memory
        // Description: The specified register or memory byte is \
           decremented by one.
        // DCR B
        case 0x05:{
            uint16_t res = state->b - 1;

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

            state->b -= 1;

            (*cycles) += 5;
            break;
        }
        
        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI B
        case 0x06: 
            state->b = opcode[1];
            state->pc += 1;

            (*cycles) += 7;
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

            (*cycles) += 4;
            break;

        case 0x08:
            break;

        // DAD Double Add
        // Description: The 16-bit number in the specified regis- \
           ter pair is added to the 16-bit number held in the Hand L \
           registers using two's complement arithmetic. The result re- \
           places the contents of the Hand L registers.    
        // DAD BC
        case 0x09:{
            uint32_t tmp = ((state->b << 8) | (state->c)) + ((state->h << 8) | state->l);
            state->h = (tmp  >> 8) & 0b11111111;
            state->l = tmp & 0b11111111;

            // carry flag
            if (tmp & 0b100000000)
                state->cc.cy = 1;
            else
                state->cc.cy = 0; 

            (*cycles) += 10;
            break;
        }

        // LDAX Load Accumulator
        // Description: The contents of the memory location \
           addressed by registers Band C, or by registers 0 and E, re- \
           place the contents of the accumulator.
        // LDAX B
        case 0x0a:
            state->a = state->memory[(state->b << 8) | state->c];

            (*cycles) += 7;
            break;

        // DCX Decrement Register Pair
        // Description: The 16-bit number held in the specified \
           register pair is decremented by one.
        // DCX BC
        case 0x0b:{
            uint16_t tmp = (state->b << 8) | state->c;
            tmp--;
            state->b = tmp >> 8;
            state->c = tmp & 0b11111111;

            (*cycles) += 5;
            break;
        }

        // INR Increment Register or Memory
        // Description: The specified register or memory byte is \
           incremented by one.
        // INR C
        case 0x0c: {
            uint16_t res = state->c + 1;

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

            state->c += 1;

            (*cycles) += 5;
            break;
        }

        // DCR Decrement Register or Memory
        // Description: The specified register or memory byte is \
           decremented by one.
        // DCR C
        case 0x0d:{
            uint16_t res = state->c - 1;

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

            state->c -= 1;

            (*cycles) += 5;
            break;
        }

        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI C
        case 0x0e: 
            state->c = opcode[1];
            state->pc += 1;
            (*cycles) += 7;
            break;

        // RRC Rotate Accumulator Right
        // RRC
        case 0x0f:
            state->cc.cy = (state->a & 0b1);
            state-> a = (state->a >> 1) | (state->cc.cy << 7);
            (*cycles) += 4;
            break;

        case 0x10: 
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
        // LXI DE
        case 0x11: 
            state->d = opcode[2];
            state->e = opcode[1]; 
            state->pc += 2;
            (*cycles) += 10;
            break;

        // STAX Store accumulator
        // Description: The contents of the accumulator are \
           stored in the memory location addressed by registers B \
           and C, or by registers D and E.
        // STAX D
        case 0x12:
            state->memory[(state->d << 8) | state->e] = state->a;
            (*cycles) += 7;
            break;

        // INX Increment Register Pair \
           Description: The 16-bit number held in the specified \
           register pair is incremented by one.
        // INX DE
        case 0x13:{
            uint16_t tmp = (state->d << 8) | state->e;
            tmp++;
            state->d = tmp >> 8;
            state->e = tmp & 0b11111111;
            (*cycles) += 5;
            break;
        }

        // INR Increment Register or Memory
        // Description: The specified register or memory byte is \
           incremented by one.
        // INR D
        case 0x14:{
            uint16_t res = state->d + 1;

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

            state->d += 1;

            (*cycles) += 5;
            break;
        }

        // DCR Decrement Register or Memory
        // Description: The specified register or memory byte is \
           decremented by one.
        // DCR D
        case 0x15:{
            uint16_t res = state->d - 1;

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

            state->d -= 1;

            (*cycles) += 5;
            break;
        }

        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI D
        case 0x16: 
            state->d = opcode[1];
            state->pc += 1;
            (*cycles) += 7;
            break;

        // RAL Rotate Accumulator Left Through Carry
        // RAL
        case 0x17: {
            uint8_t tmp = state->cc.cy;
            state->cc.cy = (state->a >> 7) & 0b1;
            state->a = (state->a << 1) | tmp;
            (*cycles) += 4;
            break;
        }

        case 0x18:
            break;

        // DAD Double Add
        // Description: The 16-bit number in the specified regis- \
           ter pair is added to the 16-bit number held in the Hand L \
           registers using two's complement arithmetic. The result re- \
           places the contents of the Hand L registers.    
        // DAD DE
        case 0x19: {
            uint32_t tmp = ((state->d << 8) | state->e) + ((state->h << 8) | state->l);
            state->h = (tmp >> 8)  & 0b11111111;
            state->l = tmp & 0b11111111;

            if (tmp > 0xffff)
                state->cc.cy = 1;
            else 
                state->cc.cy = 0;

            (*cycles) += 10;
            break;
        }

        // LDAX Load Accumulator
        // Description: The contents of the memory location \
           addressed by registers Band C, or by registers 0 and E, re- \
           place the contents of the accumulator.
        // LDAX D
        case 0x1a:
            state->a = state->memory[(state->d << 8) | state->e];
            (*cycles) += 7;
            break;

        // DCX Decrement Register Pair
        // Description: The 16-bit number held in the specified \
           register pair is decremented by one.
        // DCX DE
        case 0x1b: {
            uint16_t tmp = (state->d << 8) | state->e;
            tmp--;
            state->d = tmp >> 8;
            state->e = tmp & 0b11111111;
            (*cycles) += 5;
            break;
        }

        // INR Increment Register or Memory
        // Description: The specified register or memory byte is \
           incremented by one.
        // INR E
        case 0x1c:{
            uint16_t res = state->e + 1;

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

            state->e += 1;

            (*cycles) += 5;
            break;
        }

        // DCR Decrement Register or Memory
        // Description: The specified register or memory byte is \
           decremented by one.
        // DCR E
        case 0x1d:{
            uint16_t res = state->e -  1;

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

            state->e -= 1;

            (*cycles) += 5;
            break;
        }

        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI E
        case 0x1e: 
            state->e = opcode[1];
            state->pc += 1;
            (*cycles) += 7;
            break;
        
        // RAR Rotate Accumulator Right Through Carry
        // RAR
        case 0x1f:{
            uint8_t tmp = state->cc.cy;
            state->cc.cy = state->a & 0b1;
            state->a = (state->a >> 1) | (tmp << 7);
            (*cycles) += 4;
            break;
        }

        case 0x20:
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
        // LXI HL
        case 0x21: 
            state->h = opcode[2];
            state->l = opcode[1]; 
            state->pc += 2;
            (*cycles) += 10;
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
            (*cycles) += 16;
            break;

        // INX Increment Register Pair \
           Description: The 16-bit number held in the specified \
           register pair is incremented by one.
        // INX HL
        case 0x23:{
            uint16_t tmp = (state->h << 8) | state->l;
            tmp++;
            state->h = tmp >> 8;
            state->l = tmp & 0b11111111;
            (*cycles) += 5;
            break;
        }

        // INR Increment Register or Memory
        // Description: The specified register or memory byte is \
           incremented by one.
        // INR H
        case 0x24:{
            uint16_t res = state->h + 1;

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

            state->h += 1;

            (*cycles) += 5;
            break;
        }

        // DCR Decrement Register or Memory
        // Description: The specified register or memory byte is \
           decremented by one.
        // DCR H
        case 0x25:{
            uint16_t res = state->h - 1;

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

            state->h -= 1;

            (*cycles) += 5;
            break;
        }

        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI H
        case 0x26: 
            state->h = opcode[1];
            state->pc += 1;
            (*cycles) += 7;
            break;
        
        // DAA
        // no auxiliary carry instructions implemented
        case 0x27:{
            
            /*uint16_t tmp;
            
            if ((state->a & 0b1111) > 0b1001)
                state->a += 0b0110;
            if ((state->a >> 4) & 0b1111 > 0b1001 || state->cc.cy == 1){
                // soma 0b0110 aos 4 bits mais significativos de A
                tmp = state->a + (0b0110 << 4);
            }
            
            // carry bit
            if (tmp > 0b11111111)
                state->cc.cy = 1;
            // unaffected if no carry out
            
            // zero bit
            if (tmp & 0b11111111 == 0b0)
                state->cc.z = 1;
            else
                state->cc.z = 0;
            
            // sign flag
            if (tmp & 0b10000000)
                state->cc.s = 1;
            else
                state->cc.s = 0;

            // parity flag
            state->cc.p = parity(tmp & 0b11111111); 
            
            state->a = tmp & 0b11111111;*/
            
            
            (*cycles) += 4;
            break;
        }

        case 0x28:
            break;

        // DAD Double Add
        // Description: The 16-bit number in the specified regis- \
           ter pair is added to the 16-bit number held in the Hand L \
           registers using two's complement arithmetic. The result re- \
           places the contents of the Hand L registers.    
        // DAD HL
        case 0x29:{
            uint32_t tmp = ((state->h << 8) | state->l) << 1;
            state->h = (tmp >> 8) & 0b11111111;
            state->l = tmp & 0b11111111;

            if (tmp & 0b100000000)
                state->cc.cy = 1;
            else
                state->cc.cy = 0;

            (*cycles) += 10;
            break;
        }

        // LHLD Load HAnd L Direct
        // Description: The byte at the memory address formed \
           by concatenating HI ADD with LOW ADD replaces the con- \
           tents of the L register. The byte at the next higher memory \
           address replaces the contents of the H register
        // LHLD
        case 0x2a: 
            state->l = state->memory[(opcode[2] << 8) | opcode[1]];
            state->h = state->memory[((opcode[2] << 8) | opcode[1]) + 1];
            state->pc += 2;
            (*cycles) += 16;
            break;

        // DCX Decrement Register Pair
        // Description: The 16-bit number held in the specified \
           register pair is decremented by one.
        // DCX HL
        case 0x2b:{
            uint16_t tmp = (state->h << 8) | state->l;
            tmp--;
            state->h = tmp >> 8;
            state->l = tmp & 0b11111111;
            (*cycles) += 5;
            break;
        }

        // INR Increment Register or Memory
        // Description: The specified register or memory byte is \
           incremented by one.
        // INR L
        case 0x2c:{
            uint16_t res = state->l + 1;

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

            state->l += 1;

            (*cycles) += 5;
            break;
        }

        // DCR Decrement Register or Memory
        // Description: The specified register or memory byte is \
           decremented by one.
        // DCR L
        case 0x2d:{
            uint16_t res = state->l - 1;

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

            state->l -= 1;

            (*cycles) += 5;
            break;
        }

        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI L
        case 0x2e: 
            state->l = opcode[1];
            state->pc += 1;
            (*cycles) += 7;
            break;
        
        // CMA Complement Accumulator
        // Description: Each bit of the contents of the accumula- \
           tor is complemented (producing the one's complement)
        // CMA
        case 0x2f:
            state->a = ~state->a;
            (*cycles) += 4;
            break;

        case 0x30:
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
        // LXI SP
        case 0x31: 
            state->sp = (opcode[2] << 8) | opcode[1];
            state->pc += 2;            
            (*cycles) += 10;
            break;

        // STA Store Accumulator Direct
        // Description: The contents of the accumulator replace \
           the byte at the memory address formed by concatenating \
           HI ADO with LOW ADO.
        // STA
        case 0x32: 
            state->memory[(opcode[2] << 8) | opcode[1]] = state->a;
            state->pc += 2;
            (*cycles) += 13;
            break;

        // INX Increment Register Pair \
           Description: The 16-bit number held in the specified \
           register pair is incremented by one.
        // INX SP
        case 0x33:
            state->sp++;
            (*cycles) += 5;
            break;

        // INR Increment Register or Memory
        // Description: The specified register or memory byte is \
           incremented by one.
        // INR M (mem ref)
        case 0x34:{ 
            uint16_t res = state->memory[((state->h) << 8) | state->l]  + 1;

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

            state->memory[((state->h) << 8) | state->l]  += 1;

            (*cycles) += 10;
            break;
        }

        // DCR Decrement Register or Memory
        // Description: The specified register or memory byte is \
           decremented by one.
        // DCR M (mem ref)
        case 0x35: {
            uint16_t res = state->memory[((state->h) << 8) | state->l] - 1;

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

            state->memory[((state->h) << 8) | state->l] -= 1;

            (*cycles) += 10;
            break;
        }

        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI ref M (H + L)
        case 0x36: 
            state->memory[(state->h << 8) | state->l] = opcode[1];
            state->pc += 1;
            (*cycles) += 10;
            break;

        // STC Set Carry
        // Description: The Carry bit is set to one.
        // STC
        case 0x37:
            state->cc.cy = 1;
            (*cycles) += 4;
            break;

        case 0x38:
            break;

        // DAD Double Add
        // Description: The 16-bit number in the specified regis- \
           ter pair is added to the 16-bit number held in the Hand L \
           registers using two's complement arithmetic. The result re- \
           places the contents of the Hand L registers.    
        // DAD SP
        case 0x39:{
            uint32_t tmp = state->sp + ((state->h << 8) | state->l);
            state->h = (tmp >> 8) & 0b11111111;
            state->l = tmp & 0b11111111;

            if (tmp > 0xffff)
                state->cc.cy = 1;
            else
                state->cc.cy = 0;
            
            (*cycles) += 10;
            break;
        }

        // LDA Load Accumulator Direct
        // Description: The byte at the memory address formed \
           by concatenating HI ADD with LOW ADD replaces the con- \
           tents of the accumulator.
        // LDA
        case 0x3a:
            state->a = state->memory[(opcode[2] << 8) | opcode[1]];
            state->pc += 2;
            (*cycles) += 13;
            break;

        // DCX Decrement Register Pair
        // Description: The 16-bit number held in the specified \
           register pair is decremented by one.
        // DCX SP
        case 0x3b: 
            state->sp--;
            (*cycles) += 5;
            break;

        

        // INR Increment Register or Memory
        // Description: The specified register or memory byte is \
           incremented by one.
        // INR A
        case 0x3c:{
            uint16_t res = state->a + 1;

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

            state->a += 1;

            (*cycles) += 5;
            break;
        }

        // DCR Decrement Register or Memory
        // Description: The specified register or memory byte is \
           decremented by one.
        // DCR A
        case 0x3d:{
            uint16_t res = state->a - 1;

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

            state->a -= 1;

            (*cycles) += 5;
            break;
        }

        // Description: The byte of immediate data is stored in \
           the specified register or memory byte ;
        // MVI A
        case 0x3e: 
            state->a = opcode[1];
            state->pc += 1;
            (*cycles) += 7;
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
            
            (*cycles) += 4;
            break;
        
        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV B, B
        case 0x40: 
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV B, C
        case 0x41:
            state->b = state->c;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV B, D
        case 0x42:
            state->b = state->d;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV B, E
        case 0x43:
            state->b = state->e;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV B, H
        case 0x44:
            state->b = state->h;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV B, L
        case 0x45:
            state->b = state->l;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV B, M
        case 0x46:
            state->b = state->memory[(state->h << 8) | state->l];
            (*cycles) += 7;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV B, A
        case 0x47:
            state->b = state->a;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV C, B
        case 0x48: 
            state->c = state->b;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV C, C
        case 0x49:
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV C, D
        case 0x4a:
            state->c = state->d;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV C, E
        case 0x4b:
            state->c = state->e;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV C, H
        case 0x4c:
            state->c = state->h;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV C, L
        case 0x4d:
            state->c = state->l;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV C, M
        case 0x4e:
            state->c = state->memory[(state->h << 8) | state->l];
            (*cycles) += 7;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV C, A
        case 0x4f:
            state->c = state->a;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV D, B
        case 0x50:
            state->d = state->b;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV D, C
        case 0x51:
            state->d = state->c;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV D, D
        case 0x52:
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV D, E
        case 0x53:
        state->d = state->e;
        (*cycles) += 5;
        break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV D, H
        case 0x54:
            state->d = state->h;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV D, L
        case 0x55:
            state->d = state->l;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV D, M
        case 0x56: 
            state->d = state->memory[(state->h << 8) | state->l];
            (*cycles) += 7;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV D, A
        case 0x57: 
            state->d = state->a;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV E, B
        case 0x58:
            state->e = state->b;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV E, C
        case 0x59:
            state->e = state->c;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV E, D
        case 0x5a:
            state->e = state->d;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV E, E
        case 0x5b:
            (*cycles) += 5;
            break;
        
        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV E, H
        case 0x5c:
            state->e = state->h;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV E, L
        case 0x5d:
            state->e = state->l;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV E, M
        case 0x5e:
            state->e = state->memory[(state->h << 8) | state->l];
            (*cycles) += 7;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV E, A
        case 0x5f:
            state->e = state->a;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV H, B
        case 0x60:
            state->h = state->b;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV H, C
        case 0x61:
            state->h = state->c;
            (*cycles) += 5;
            break;
        
        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV H, D
        case 0x62:
            state->h = state->d;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV H, E
        case 0x63:
            state->h = state->e;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV H, H
        case 0x64:
            (*cycles) += 5;
            break;
        
        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV H, L
        case 0x65:
            state->h = state->l;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV H, M
        case 0x66:
            state->h = state->memory[(state->h << 8) | state->l];
            (*cycles) += 7;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV H, A
        case 0x67:
            state->h = state->a;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV L, B
        case 0x68:
            state->l = state->b;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV L, C
        case 0x69:
            state->l = state->c;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV L, D
        case 0x6a:
            state->l = state->d;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV L, E
        case 0x6b:
            state->l = state->e;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV L, H
        case 0x6c: 
            state->l = state->h;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV L, L
        case 0x6d: 
            (*cycles) += 5;
            break;
        
        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV L, M
        case 0x6e:
            state->l = state->memory[(state->h << 8) | state->l];
            (*cycles) += 7;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV L, A
        case 0x6f:
            state->l = state->a;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV M, B
        case 0x70: 
            state->memory[(state->h << 8) | state->l] = state->b;
            (*cycles) += 7;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV M, C
        case 0x71: 
            state->memory[(state->h << 8) | state->l] = state->c;
            (*cycles) += 7;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV M, D
        case 0x72: 
            state->memory[(state->h << 8) | state->l] = state->d;
            (*cycles) += 7;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV M, E
        case 0x73:
            state->memory[(state->h << 8) | state->l] = state->e;
            (*cycles) += 7;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV M, H
        case 0x74: 
            state->memory[(state->h << 8) | state->l] = state->h;
            (*cycles) += 7;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV M, L
        case 0x75: 
            state->memory[(state->h << 8) | state->l] = state->l;
            (*cycles) += 7;
            break;

        // HLT Halt Instruction
        // Description: The program counter is incremented to \
           the address of the next sequential instruction. The CPU then \
           enters the STOPPED state and no further activity takes \
           place until an interrupt occurs.
        // HLT
        case 0x76:
            (*cycles) += 7;
            exit(0);

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV M, A
        case 0x77:
            state->memory[(state->h << 8) | state->l] = state->a;
            (*cycles) += 7;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV A, B
        case 0x78:
            state->a = state->b;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV A, C
        case 0x79:
            state->a = state->c;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV A, D
        case 0x7a: 
            state->a = state->d;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV A, E
        case 0x7b: 
            state->a = state->e;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV A, H
        case 0x7c: 
            state->a = state->h;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV A, L
        case 0x7d:  
            state->a = state->l;
            (*cycles) += 5;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV A, M
        case 0x7e: 
            state->a = state->memory[(state->h << 8) | state->l];
            (*cycles) += 7;
            break;

        // MOV
        // Description: One byte of data is moved from the \
           register specified by src (the source register) to the register \
           specified by dst (the destination register). The data re- \
           places the contents of the destination register; the source \
           remains unchanged.
        // MOV A, A
        case 0x7f: 
            state->a = state->a;
            (*cycles) += 5;
            break;
        
        // Instructions in this class operate on the accumulator \
           using the byte in the register specified by REG. If a memory \
           reference is specified, the instructions use the byte in the \
           memory location addressed by registers Hand L. The H reg- \
           ister holds the most significant 8 bits of the address, while \
           the L register holds the least significant 8 bits of the address. 
        // Description: The specified byte is added to the con· \
           tents of the accumulator using two's complement arithmetic.
        // ADD B
        case 0x80:{
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
            
            (*cycles) += 4;
            break;
        }

        // Instructions in this class operate on the accumulator \
           using the byte in the register specified by REG. If a memory \
           reference is specified, the instructions use the byte in the \
           memory location addressed by registers Hand L. The H reg- \
           ister holds the most significant 8 bits of the address, while \
           the L register holds the least significant 8 bits of the address. 
        // Description: The specified byte is added to the con· \
           tents of the accumulator using two's complement arithmetic.
        // ADD C
        case 0x81: {
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
            
            (*cycles) += 4;
            break;
        }

        // Instructions in this class operate on the accumulator \
           using the byte in the register specified by REG. If a memory \
           reference is specified, the instructions use the byte in the \
           memory location addressed by registers Hand L. The H reg- \
           ister holds the most significant 8 bits of the address, while \
           the L register holds the least significant 8 bits of the address. 
        // Description: The specified byte is added to the con· \
           tents of the accumulator using two's complement arithmetic.
        // ADD D
        case 0x82:{
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
            
            (*cycles) += 4;
            break;
        }

        // Instructions in this class operate on the accumulator \
           using the byte in the register specified by REG. If a memory \
           reference is specified, the instructions use the byte in the \
           memory location addressed by registers Hand L. The H reg- \
           ister holds the most significant 8 bits of the address, while \
           the L register holds the least significant 8 bits of the address. 
        // Description: The specified byte is added to the con· \
           tents of the accumulator using two's complement arithmetic.
        // ADD E
        case 0x83:{
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
            
            (*cycles) += 4;
            break;
        }

        // Instructions in this class operate on the accumulator \
           using the byte in the register specified by REG. If a memory \
           reference is specified, the instructions use the byte in the \
           memory location addressed by registers Hand L. The H reg- \
           ister holds the most significant 8 bits of the address, while \
           the L register holds the least significant 8 bits of the address. 
        // Description: The specified byte is added to the con· \
           tents of the accumulator using two's complement arithmetic.
        // ADD H
        case 0x84: {
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
            
            (*cycles) += 4;
            break;
        }

        // Instructions in this class operate on the accumulator \
           using the byte in the register specified by REG. If a memory \
           reference is specified, the instructions use the byte in the \
           memory location addressed by registers Hand L. The H reg- \
           ister holds the most significant 8 bits of the address, while \
           the L register holds the least significant 8 bits of the address. 
        // Description: The specified byte is added to the con· \
           tents of the accumulator using two's complement arithmetic.
        // ADD L
        case 0x85:{
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
            
            (*cycles) += 4;
            break;
        }

        // Instructions in this class operate on the accumulator \
           using the byte in the register specified by REG. If a memory \
           reference is specified, the instructions use the byte in the \
           memory location addressed by registers Hand L. The H reg- \
           ister holds the most significant 8 bits of the address, while \
           the L register holds the least significant 8 bits of the address. 
        // Description: The specified byte is added to the con· \
           tents of the accumulator using two's complement arithmetic.
        // ADD M (memory reference)
        case 0x86:{
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
            
            (*cycles) += 7;
            break;
        }

        // Instructions in this class operate on the accumulator \
           using the byte in the register specified by REG. If a memory \
           reference is specified, the instructions use the byte in the \
           memory location addressed by registers Hand L. The H reg- \
           ister holds the most significant 8 bits of the address, while \
           the L register holds the least significant 8 bits of the address. 
        // Description: The specified byte is added to the con· \
           tents of the accumulator using two's complement arithmetic.
        // ADD A
        case 0x87:{
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
            
            (*cycles) += 4;
            break; 
        }

        // Just like ADD, but addas carry out in the LSB if set
        // ADC B
        case 0x88: {
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

            (*cycles) += 4;            
            break;  
        }

        // Just like ADD, but addas carry out in the LSB if set
        // ADC C
        case 0x89: {
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
            
            (*cycles) += 4;
            break;  
        }

        // Just like ADD, but addas carry out in the LSB if set
        // ADC D
        case 0x8a: {
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
            
            (*cycles) += 4;
            break;  
        }

        // Just like ADD, but addas carry out in the LSB if set
        // ADC E
        case 0x8b: {
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
            
            (*cycles) += 4;
            break;  
        }

        // Just like ADD, but addas carry out in the LSB if set
        // ADC H
        case 0x8c: {
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
            
            (*cycles) += 4;
            break;  
        }

        // Just like ADD, but addas carry out in the LSB if set
        // ADC L
        case 0x8d: {
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
            
            (*cycles) += 4;
            break;  
        }

        // Just like ADD, but addas carry out in the LSB if set
        // ADC M (mem ref)
        case 0x8e: {
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
            
            (*cycles) += 7;
            break;  
        }

        // Just like ADD, but addas carry out in the LSB if set
        // ADC A
        case 0x8f:{
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
            
            (*cycles) += 4;
            break; 
        }
        
        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB B
        case 0x90: {
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
            state->cc.cy = (state->a < state->b) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;

            (*cycles) += 4;
            break;
        }

        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB C
        case 0x91: {
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
            state->cc.cy = (state->a < state->c) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;

            (*cycles) += 4;
            break;
        }

        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB D
        case 0x92: {
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
            state->cc.cy = (state->a < state->d) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;

            (*cycles) += 4;
            break;
        }

        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB E
        case 0x93: {
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
            state->cc.cy = (state->a < state->e) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;

            (*cycles) += 4;
            break;
        }

        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB H
        case 0x94: {
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
            state->cc.cy = (state->a < state->h) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;

            (*cycles) += 4;
            break;
        }

        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB L
        case 0x95: {
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
            state->cc.cy = (state->a < state->l) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;

            (*cycles) += 4;
            break;
        }

        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB M (mem ref)
        case 0x96: {
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
            state->cc.cy = (state->a < state->memory[(state->h << 8) | state->l]) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;

            (*cycles) += 7;
            break;
        }

        // Description: The specified byte is subtracted from the \
           accumulator using two's complement arithmetic.
        // SUB A
        case 0x97: {
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
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;

            (*cycles) += 4;
            break;
        }

        // SBB Subtract Register or Memory From \
           Accumulator With Borrow
        // Description: The Carry bit is internally added to the \
           contents of the specified byte. This value is then subtracted \
           from the accumulator using two's complement arithmetic.
        // SBB B
        case 0x98: {
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
            state->cc.cy = (state->a < (state->b + state->cc.cy)) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;

            (*cycles) += 4;
            break;
        }

        // SBB Subtract Register or Memory From \
           Accumulator With Borrow
        // Description: The Carry bit is internally added to the \
           contents of the specified byte. This value is then subtracted \
           from the accumulator using two's complement arithmetic.
        // SBB C
        case 0x99:{
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
            state->cc.cy = (state->a < (state->c + state->cc.cy)) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;

            (*cycles) += 4;
            break;
        }

        // SBB Subtract Register or Memory From \
           Accumulator With Borrow
        // Description: The Carry bit is internally added to the \
           contents of the specified byte. This value is then subtracted \
           from the accumulator using two's complement arithmetic.
        // SBB D
        case 0x9a: {
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
            state->cc.cy = (state->a < (state->d + state->cc.cy)) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;

            (*cycles) += 4;
            break;
        }

        // SBB Subtract Register or Memory From \
           Accumulator With Borrow
        // Description: The Carry bit is internally added to the \
           contents of the specified byte. This value is then subtracted \
           from the accumulator using two's complement arithmetic.
        // SBB E
        case 0x9b: {
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
            state->cc.cy = (state->a < (state->e + state->cc.cy)) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;

            (*cycles) += 4;
            break;
        }

        // SBB Subtract Register or Memory From \
           Accumulator With Borrow
        // Description: The Carry bit is internally added to the \
           contents of the specified byte. This value is then subtracted \
           from the accumulator using two's complement arithmetic.
        // SBB H
        case 0x9c:{
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
            state->cc.cy = (state->a < (state->h + state->cc.cy)) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;

            (*cycles) += 4;
            break;
        }

        // SBB Subtract Register or Memory From \
           Accumulator With Borrow
        // Description: The Carry bit is internally added to the \
           contents of the specified byte. This value is then subtracted \
           from the accumulator using two's complement arithmetic.
        // SBB L
        case 0x9d: {
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
            state->cc.cy = (state->a < (state->l + state->cc.cy)) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;

            (*cycles) += 4;
            break;
        }

        // SBB Subtract Register or Memory From \
           Accumulator With Borrow
        // Description: The Carry bit is internally added to the \
           contents of the specified byte. This value is then subtracted \
           from the accumulator using two's complement arithmetic.
        // SBB M (mem ref)
        case 0x9e: {
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
            state->cc.cy = (state->a < state->memory[(state->h << 8) | state->l] + state->cc.cy) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;

            (*cycles) += 7;
            break;
        }

        // SBB Subtract Register or Memory From \
           Accumulator With Borrow
        // Description: The Carry bit is internally added to the \
           contents of the specified byte. This value is then subtracted \
           from the accumulator using two's complement arithmetic.
        // SBB A
        case 0x9f: {
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
            state->cc.cy = (state->a < (state->a + state->cc.cy)) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            // put result into accumulator
            state->a = res & 0b11111111;

            (*cycles) += 4;
            break;
        }

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

            (*cycles) += 4;
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

            (*cycles) += 4;
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

            (*cycles) += 4;
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

            (*cycles) += 4;
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

            (*cycles) += 4;
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

            (*cycles) += 4;
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

            (*cycles) += 7;
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

            (*cycles) += 4;
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

            (*cycles) += 4;
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

            (*cycles) += 4;
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

            (*cycles) += 4;
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

            (*cycles) += 4;
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

            (*cycles) += 4;
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

            (*cycles) += 4;
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

            (*cycles) += 7;
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

            (*cycles) += 4;
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

            (*cycles) += 4;
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

            (*cycles) += 4;
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

            (*cycles) += 4;
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

            (*cycles) += 4;
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

            (*cycles) += 4;
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

            (*cycles) += 4;
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

            (*cycles) += 7;
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

            (*cycles) += 4;
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
        case 0xb8: {
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
            state->cc.cy = (state->a < state->b) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            (*cycles) += 4;
            break;
        }

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
        case 0xb9: {
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
            state->cc.cy = (state->a < state->c) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            (*cycles) += 4;
            break;
        }

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
        case 0xba: {
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
            state->cc.cy = (state->a < state->d) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            (*cycles) += 4;
            break;
        }

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
        case 0xbb:{
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
            state->cc.cy = (state->a < state->e) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            (*cycles) += 4;
            break;
        }

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
        case 0xbc: {
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
            state->cc.cy = (state->a < state->h) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            (*cycles) += 4;
            break;
        }

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
        case 0xbd: {
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
            state->cc.cy = (state->a < state->l) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            (*cycles) += 4;
            break;
        }

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
        case 0xbe: {
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
            state->cc.cy = (state->a < state->memory[(state->h << 8) | state->l]) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            (*cycles) += 7;
            break;
        }

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
        case 0xbf:{
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
            state->cc.cy = (state->a < state->a) ? 1 : 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            (*cycles) += 4;
            break;
        }

        // RNZ Return If Not Zero
        // Description: If the Zero bit is zero, a return operation \
           is performed.
        // RNZ
        case 0xc0:
            if (!(state->cc.z)){
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
                state->pc--;
                state->sp += 2;
            }
            (*cycles) += 11/5;
            break;
        // POP Pop Data Off Stack
        // POP BC
        case 0xc1:
            state->c = state->memory[state->sp];
            state->b = state->memory[state->sp+1];
            state->sp += 2;
            (*cycles) += 10;
            break;

        // JNZ Jump If Not Zero 
        // Description: If the Zero bit is zero, program execu- \
           tion continues at the memory address adr.
        // JNZ
        case 0xc2:
            if (!(state->cc.z)){
                state->pc = (opcode[2] << 8) | opcode[1];
                state->pc--;
            }
            else
                state->pc += 2;

            (*cycles) += 10;
            break;

        // JMP JUMP
        // Description: Program execution continues uncondi- \
           tionally at memory address adr.
        // JMP
        case 0xc3:
            state->pc =(opcode[2] << 8) | opcode[1];
            state->pc--;
            (*cycles) += 10;
            break;

        // CNZ Call If Not Zero
        // Description: If the Zero bit is one, a call operation is \
           performed to subroutine sub.
        // CNZ
        case 0xc4:
            if (!state->cc.z){
                state->memory[state->sp - 1] = ((state->pc+3) >> 8) & 0b11111111;
                state->memory[state->sp - 2] =  (state->pc+3) & 0b11111111;
                state->sp -= 2;
                state->pc = ((opcode[2]) << 8) | opcode[1];
                state->pc--;
            }
            else
                state->pc += 2;
            
            (*cycles) += 17/11;
            break;
        // PUSH Push Data Onto Stack
        // Description: The contents of the specified register pair \
           are saved in two bytes of memory indicated by the stack  \
           pointer SP.
        // PUSH BC
        case 0xc5:
            state->memory[state->sp-1] = state->b;
            state->memory[state->sp-2] = state->c;
            state->sp -= 2;
            (*cycles) += 11;
            break;

        // Description: The byte of immediate data is added to \
           the contents of the accumulator using two's complement \
           arithmetic.
        // ADI
        case 0xc6: {
            uint16_t res = (uint16_t) state->a + (uint16_t) opcode[1];
            
            // zero flag
            if ((res & 0b11111111) == 0b0)
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

            state->cc.p = parity(res & 0b11111111);

            state->a = res & 0b11111111;            

            state->pc += 1;

            (*cycles) += 7;
            break;
        }

        // RST 0b000
        case 0xc7:
            GenerateInterrupt(state, (*opcode >> 3) & 0b111);
            (*cycles) += 11;
            break;
        
        // RZ Return If Zero
        // Description: If the Zero bit is one, a return operation \
           is performed.
        // RZ   
        case 0xc8:
            if (state->cc.z){
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
                state->pc--;
                state->sp += 2;
            }
            (*cycles) += 11/5;
            break;

        // RET Return
        // Description: A return operation is unconditionally \
           performed.
        // RET
        case 0xc9:
            // first byte is LSB, second is MSB
            state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
            state->pc--;
            state->sp += 2;
            (*cycles) += 10;
            break;

        // JZ Jump If Zero
        // Description: If the zero bit is one, program execution \
           continues at the memory address adr.
        // JZ
        case 0xca:
            if (state->cc.z){
                state->pc = (opcode[2] << 8) | opcode[1];
                state->pc--;
            }
            else
                state->pc += 2;

            (*cycles) += 10;
            break;

        case 0xcb: UnimplementedInstruction(state); (*cycles) += 10; break;

        // CZ Call If Zero
        // Description: If the Zero bit is zero, a call operation is \
           performed to subroutine sub.
        // CZ
        case 0xcc:
            if (state->cc.z){
                state->memory[state->sp - 1] = ((state->pc+3) >> 8) & 0b11111111;
                state->memory[state->sp - 2] =  (state->pc+3) & 0b11111111;
                state->sp -= 2;
                state->pc = (opcode[2] << 8) | opcode[1];
                state->pc--;
            }
            else
                state->pc += 2;
            
            (*cycles) += 17/11;
            break;

        // CALL Call
        // Description: A call operation is unconditionally per- \
           formed to subroutine sub.
        // CALL
        case 0xcd:{
            state->memory[state->sp - 1] = ((state->pc+3) >> 8) & 0b11111111;
            state->memory[state->sp - 2] =  (state->pc+3) & 0b11111111;
            state->sp -= 2;
            state->pc = (opcode[2] << 8) | opcode[1];
            state->pc--;


            (*cycles) += 17;
            break;
        }

        // ACI Add Immediate To Accumulator With Carry
        // Description: The byte of immediate data is added to \
           the contents of the accumulator plus the contents of the \
           carry bit.
        // ACI
        case 0xce:{
            uint16_t res = (uint16_t) state->a + (uint16_t) opcode[1] + state->cc.cy;

            // zero flag
            if ((res & 0b11111111) == 0b0)
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

            state->cc.p = parity(res & 0b11111111);

            state->a = res & 0b11111111;            

            state->pc += 1;

            (*cycles) += 7;
            break;
        }

        // RST 0b001
        case 0xcf:
            GenerateInterrupt(state, (*opcode >> 3) & 0b111);
            (*cycles) += 11;
            break;

        // RNC Return If No Carry
        // Description: If the carry bit is zero, a return operation \
           is performed.
        // RNC
        case 0xd0:
            if (!(state->cc.cy)){
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
                state->pc--;
                state->sp += 2;
            }
            
            (*cycles) += 11/5;
            break;            

        // POP Pop Data Off Stack
        // POP DE
        case 0xd1:
            state->e = state->memory[state->sp];
            state->d = state->memory[state->sp+1];
            state->sp += 2;
            (*cycles) += 10;
            break;

        // JNC Jump If No Carry
        // Description: If the Carry bit is zero, program execu- \
           tion continues at the memory address adr.
        // JNC
        case 0xd2: 
            if (!(state->cc.cy)){
                state->pc = (opcode[2] << 8) | opcode[1];
                state->pc--;
            }
            else
                state->pc += 2;
            
            (*cycles) += 10;
            break;

        // OUT Output
        // Description: The contents of the accumulator are sent \
           to output device number expo
        // OUT
        case 0xd3:
            // opcode[1] = port
            MachineOUT(state, opcode[1]);
            state->pc += 1;
            (*cycles) += 10;
            break;

        // CNC Call If No Carry
        // Description: If the Carry bit is zero, a call operation is \
           performed to subroutine sub.
        // CNC
        case 0xd4: 
            if (!(state->cc.cy)){
                state->memory[state->sp - 1] = ((state->pc+3) >> 8) & 0b11111111;
                state->memory[state->sp - 2] =  (state->pc+3) & 0b11111111;
                state->sp -= 2;
                state->pc = (opcode[2] << 8) | opcode[1];
                state->pc--;
            }
            else
                state->pc += 2;

            (*cycles) += 17/11;
            break;
        // PUSH Push Data Onto Stack
        // Description: The contents of the specified register pair \
           are saved in two bytes of memory indicated by the stack  \
           pointer SP.
        // PUSH DE
        case 0xd5: 
            state->memory[state->sp-1] = state->d;
            state->memory[state->sp-2] = state->e;
            state->sp -= 2;
            (*cycles) += 11;
            break;

        // SUI Subtract Immediate From Accumulator
        // Description: The byte of immediate data is subtracted \
           from the contents of the accumulator using two's comple- \
           ment arithmetic. \
           Since this is a subtraction operation, the carry bit is \
           set, indicati ng a borrow, if there is no carry out of the high- \
           order bit position, and reset if there is a carry out.
        // SUI
        case 0xd6: {
            uint16_t res = (uint16_t) state->a - (uint16_t) opcode[1];

            // zero flag
            if ((res & 0b11111111) == 0b0)
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

            state->a = res & 0b11111111;            

            state->pc += 1;

            (*cycles) += 7;
            break;
        }

        // RST 0b010
        case 0xd7: 
            GenerateInterrupt(state, (*opcode >> 3) & 0b111);
            (*cycles) += 11;
            break;

        // RC Return If Carry
        // Description: If the Carry bit is one, a return operation \
           is performed.
        // RC
        case 0xd8:
            if (state->cc.cy){
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
                state->pc--;
                state->sp += 2;
            }

            (*cycles) += 11/5;
            break;

        case 0xd9: UnimplementedInstruction(state); (*cycles) += 10; break;

        // JC Jump If Carry
        // Description: If the Carry bit is one, program execu- \
           tion conti nues at the memory address adr.
        // JC
        case 0xda:
            if (state->cc.cy){
                state->pc = (opcode[2] << 8) | opcode[1];
                state->pc--;
            }
            else
                state->pc += 2;
            
            (*cycles) += 10;
            break;

        // IN Input
        // Description: An eight-bit data byte is read from input \
           device number exp and replaces the contents of the \
           accumulator.
        // IN
        case 0xdb:{
            // opcode[1] = port
            state->a = MachineIN(state, opcode[1]);
            state->pc += 1;
            (*cycles) += 10;
            break;
        }

        // CC Call If Carry
        // Description: If the Carry bit is one, a call operation is \
           performed to subroutine sub.
        // CC
        case 0xdc:
            if (state->cc.cy){
                state->memory[state->sp - 1] = ((state->pc+3) >> 8) & 0b11111111;
                state->memory[state->sp - 2] =  (state->pc+3) & 0b11111111;
                state->sp -= 2;
                state->pc = ((opcode[2]) << 8) | opcode[1];
                state->pc--;
            }
            else
                state->pc += 2;
            (*cycles) += 17/11;
            break;

        case 0xdd: UnimplementedInstruction(state); (*cycles) += 17; break;

        // SBI Subtract Immediate from Accumulator \
           With Borrow
        // Description: The Carry bit is internally added to the \
           byte of immediate data. This value is then subtracted from \
           the accumulator using two's complement arithmetic.
        // SBI
        case 0xde: {
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
                state->cc.cy = 1;
            else
                state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            state->a = res & 0b11111111;            

            state->pc += 1;

            (*cycles) += 7;
            break;
        }


        // RST 0b011
        case 0xdf:
            GenerateInterrupt(state, (*opcode >> 3) & 0b111);
            (*cycles) += 11;
            break;

        // RPO Return If Parity Odd
        // Description: If the Parity bit is zero (indicating odd \
           parity), a return operation is performed.
        // RPO
        case 0xe0:
            if (!(state->cc.p)){
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
                state->pc--;
                state->sp += 2;
            }
            (*cycles) += 11/5;
            break;

        // POP Pop Data Off Stack
        // POP HL
        case 0xe1:
            state->l = state->memory[state->sp];
            state->h = state->memory[state->sp+1];
            state->sp += 2;
            (*cycles) += 10;
            break;

        // JPO Jump If Parity Odd
        // Description: If the Parity bit is zero (indicating a re- \
           sult with odd parity), program execution conti nues at the \
           memory address adr.
        // JPO
        case 0xe2:
            if (!(state->cc.p)){
                state->pc = (opcode[2] << 8) | opcode[1];
                state->pc--;
            }
            else
                state->pc += 2;
            
            (*cycles) += 10;
            break;
        // XTHL Exchange Stack
        // Description: The contents of the L register are ex- \
           changed with the contents of the memory byte whose ad- \
           dress is held in the stack pointer SP. The contents of the H \
           register are exchanged with the contents of the memory \
           byte whose address is one greater than that held in the stack \
           pointer.
        // XTHL
        case 0xe3:{
            uint8_t L = state->l;
            uint8_t H = state->h;

            state->l = state->memory[state->sp];
            state->h = state->memory[state->sp+1];
            state->memory[state->sp] = L;
            state->memory[state->sp+1] = H;
            (*cycles) += 10;
            break;
        }

        // CPO Call If Parity Odd
        // Description: If the Parity bit is zero (indicating odd \
           parity), a call operation is performed to subroutine sub.
        // CPO
        case 0xe4:
            if (!(state->cc.p)){
                state->memory[state->sp - 1] = ((state->pc+3) >> 8) & 0b11111111;
                state->memory[state->sp - 2] =  (state->pc+3) & 0b11111111;
                state->sp -= 2;
                state->pc = ((opcode[2]) << 8) | opcode[1];
                state->pc--;
            }
            else
                state->pc += 2;
            
            (*cycles) += 17/11;
            break;
        // PUSH Push Data Onto Stack
        // Description: The contents of the specified register pair \
           are saved in two bytes of memory indicated by the stack  \
           pointer SP.
        // PUSH HL
        case 0xe5:
            state->memory[state->sp-1] = state->h;
            state->memory[state->sp-2] = state->l;
            state->sp -= 2;
            (*cycles) += 11;
            break;

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
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            state->pc += 1;

            (*cycles) += 7;
            break;

        // RST 0b100
        case 0xe7:
            GenerateInterrupt(state, (*opcode >> 3) & 0b111);
            (*cycles) += 11;
            break;

        // RPE Return If Parity Even
        // Description: If the Parity bit is one (indicating even \
           parity), a return operation is performed.
        // RPE
        case 0xe8:
            if (state->cc.p){
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
                state->pc--;
                state->sp += 2;
            }
            (*cycles) += 11/5;
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
            state->pc--;
            (*cycles) += 5;
            break;
        
        // JPE Jump If Parity Even
        // Description: If the parity bit is one (indicating a result \
           with even parity), program execution continues at the mem- \
           ory address adr.
        // JPE
        case 0xea: 
            if (state->cc.p){
                state->pc = (opcode[2] << 8) | opcode[1];
                state->pc--;
            }
            else
                state->pc += 2;

            (*cycles) += 10;
            break;

        // XCHG Exchange Registers
        // Description: The 16 bits of data held in the H and L \
           registers are exchanged with the 16 bits of data held in the \
           D and E registers.
        // XCHG
        case 0xeb:{
            uint8_t H = state->h;
            uint8_t L = state->l;

            state->h = state->d;
            state->l = state->e;
            state->d = H;
            state->e = L;

            (*cycles) += 5;
            break;
        }

        // CPE Call If Parity Even
        // Description: If the Parity bit is one (indicating even \
           parity), a call operation is performed to subroutine sub.
        // CPE
        case 0xec:
            if (state->cc.p){
                state->memory[state->sp - 1] = ((state->pc+3) >> 8) & 0b11111111;
                state->memory[state->sp - 2] =  (state->pc+3) & 0b11111111;
                state->sp -= 2;
                state->pc = ((opcode[2]) << 8) | opcode[1];
                state->pc--;
            }
            else
                state->pc += 2;
            
            (*cycles) += 17/11;
            break;

        case 0xed: UnimplementedInstruction(state); (*cycles) += 17; break;

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
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            state->pc += 1;

            (*cycles) += 7;
            break;

        // RST 0b101
        case 0xef:
            GenerateInterrupt(state, (*opcode >> 3) & 0b111);
            (*cycles) += 11;
            break;

        // RP Return If Plus
        // Description: If the Sign bit is zero (indicating a posi- \
           tive result). a return operation is performed.
        // RP
        case 0xf0: 
            if (!(state->cc.s)){
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
                state->pc--;
                state->sp += 2;
            }
            (*cycles) += 11/5;
            break;

        // POP Pop Data Off Stack
        // POP PSW
        case 0xf1:{
            uint8_t fl = state->memory[state->sp];
            state->a = state->memory[state->sp+1];
            
            state->cc.s = fl >> 7;
            state->cc.z = (fl >> 6) & 0b1;
            state->cc.p = (fl >> 2) & 0b1;
            state->cc.cy = fl & 0b1;

            state->sp += 2;
            (*cycles) += 10;
            break;
        }

        // JP Jump If Positive
        // Description: If the sign bit is zero, (indicating a posi- \
           tive result), program execution continues at the memory \
           address adr.
        // JP
        case 0xf2:
            if (!(state->cc.s)){
                state->pc = (opcode[2] << 8) | opcode[1];
                state->pc--;
            }
            else
                state->pc += 2;

            (*cycles) += 10;
            break;

        // DI Disable Interrupts
        // Descriptio: This instruction resets the INTE flip-flop, \
           causing the CPU to ignore all interrupts.
        // DI
        case 0xf3:
            state->int_enable = 0;
            (*cycles) += 4;
            break;

        // CP Call If Plus
        // Description: If the Sign bit is zero (indicating a posi- \
           tive result), a call operation is performed to subroutine sub.
        // CP
        case 0xf4:
            if (!(state->cc.s)){
                state->memory[state->sp - 1] = ((state->pc+3) >> 8) & 0b11111111;
                state->memory[state->sp - 2] =  (state->pc+3) & 0b11111111;
                state->sp -= 2;
                state->pc = ((opcode[2]) << 8) | opcode[1];
                state->pc--;
            }
            else
                state->pc += 2;

            (*cycles) += 17/11;
            break;

        // PUSH Push Data Onto Stack
        // Description: The contents of the specified register pair \
           are saved in two bytes of memory indicated by the stack  \
           pointer SP.
        // PUSH PSW
        case 0xf5: 
            state->memory[state->sp-1] = state->a;
            state->memory[state->sp-2] = ((state->cc.s << 7) | (state->cc.z << 6) | 
                                            (state->cc.p << 2) | (0b10) | (state->cc.cy));
            state->sp -= 2;
            (*cycles) += 11;
            break;

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
            state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(state->a);

            state->pc += 1;

            (*cycles) += 7;
            break;

        // RST 0b110
        case 0xf7:
            GenerateInterrupt(state, (*opcode >> 3) & 0b111);
            (*cycles) += 11;
            break;

        // RM Return If Minus
        // Description: If the Sign bit is one (indicating a minus \
           result), a return operation is performed.
        // RM
        case 0xf8:
            if (state->cc.s){
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
                state->pc--;
                state->sp += 2;
            }

            (*cycles) += 11/5;
            break;

        // SPHL Load SP From HAnd L
        // Description: The 16 bits of data held in the Hand L \
           registers replace the contents of the stack pointer SP. The \
           contents of the Hand L registers are unchanged.
        // SPHL
        case 0xf9:
            state->sp = (state->h << 8) | state->l;
            (*cycles) += 5;
            break;

        // JM Jump If Minus
        // Description: If the Sign bit is one (indicating a nega- \
           tive result), program execution continues at the memory \
           address adr.
        // JM
        case 0xfa:
            if (state->cc.s){
                state->pc = (opcode[2] << 8) | opcode[1];
                state->pc--;
            }
            else
                state->pc += 2;

            (*cycles) += 10;
            break;
        // EI Enable Interrupts
        // Description: This instruction sets the I NTE flip-flop, \
           enabling the CPU to recognize and respond to interrupts
        // EI
        case 0xfb:
            state->int_enable = 1;
            (*cycles) += 4;
            break;

        // CM Call If Minus
        // Description: If the Sign bit is one (indicating a minus \
           result), a call operation is performed to subrouti ne sub.
        // CM
        case 0xfc: 
            if (state->cc.s){
                state->memory[state->sp - 1] = ((state->pc+3) >> 8) & 0b11111111;
                state->memory[state->sp - 2] =  (state->pc+3) & 0b11111111;
                state->sp -= 2;
                state->pc = ((opcode[2]) << 8) | opcode[1];
                state->pc--;
            }
            else
                state->pc += 2;
            
            (*cycles) += 17/11;
            break;

        case 0xfd: UnimplementedInstruction(state); (*cycles) += 17; break;

        // CPI Compare Immediate With Accumulator
        // Description: The byte of immediate data is compared \
           to the contents of the accumulator.
        // CPI
        case 0xfe:{
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
            // Since a subtract operation is performed, the Carry bit \
               will be set if there is no carry out of bit 7, indicating the \
               immediate data is greater than the contents of the accumu· \
               lator, and reset otherwise.
            if (state->a < opcode[1])
                state->cc.cy = 1;
            else
                state->cc.cy = 0;

            // parity flag
            state->cc.p = parity(res & 0b11111111);

            state->pc += 1;

            (*cycles) += 7;
            break;
        }

        // RST 0b111
        case 0xff:
            GenerateInterrupt(state, (*opcode >> 3) & 0b111);
            (*cycles) += 11;
            break;

        // if no instruction is found
        default:
            printf("ERROR. Instruction not found \nstate->pc = %d \n*opcode = %d", state->pc, *opcode);
            exit(1);
            break;
    }
    state->pc++;
}


int main(int argc, char *argv[]){
    if (argc < 2){
        printf("ROM file is missing\n");
        exit(1);
    }

    FILE *f;
    f = fopen(argv[1], "rb");
    if (f == NULL){
        printf("Error opening file");
        exit(EXIT_FAILURE);
    }    

    // find file size and put it into buffer
    fseek(f, 0L, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);

    // allocating memory for information to be stored from file
    unsigned char *buffer = (unsigned char *)malloc(fsize);
    if (buffer == NULL){
        printf("Error alocating buffer");
        fclose(f);
        exit(1);
    }

    fread(buffer, fsize, 1, f);
    fclose(f);

    // initializes other emulator
    emu8080 *emu = (emu8080 *)malloc(sizeof(emu8080));
    if (emu == NULL){
        printf("error, emu has not initialized properly\n");
        exit(1);
    }
    emu->memory = (uint8_t *)malloc(65536);
    

    // allocating memory for the emulator's state
    State8080 *state = (State8080 *)malloc(sizeof(State8080));
    if (state == NULL){
        printf("state has not initialized properly\n");
        free(state);
        free(buffer);
        exit(1);
    }
    // writing ROM into other emu memory
    for (int i = 0; i < fsize; i ++)
        emu->memory[i + 0x100] = buffer[i];

    // allocating memory for emulator, 16 kb
    state->memory = (uint8_t *)malloc(65536);
    if (state->memory == NULL){
        printf("Error allocating emulator's memory\n");
        free(state);
        free(buffer);
        exit(1);
    }

    // initizlizing emulated hardware and condition flags
    state->cc.z = 0;  
    state->cc.s = 0;  
    state->cc.p = 0;  
    state->cc.cy = 0; 
    state->cc.ac = 0; 
    state->cc.pad = 0;
    state->a = 0;
    state->b = 0;
    state->c = 0;
    state->d = 0;
    state->e = 0;
    state->h = 0;
    state->l = 0;
    state->sp = 0;

    // initializing emu hardware
    emu->a = 0;
    emu->b = 0;
    emu->c = 0;
    emu->d = 0;
    emu->e = 0;
    emu->h = 0;
    emu->l = 0;
    emu->sp = 0;
    emu->pc = 0;
    emu->cc.ac= 0;
    emu->cc.cy = 0;
    emu->cc.p = 0;
    emu->cc.s = 0;
    emu->cc.z = 0;

    // writing ROM into memory
    for (int i = 0; i < fsize; i ++)
        state->memory[i + 0x100] = buffer[i];
    
    free(buffer);

    // some other initialization steps
    // we need the first thing to happen be jumping to the code at 0x100
    emu->memory[0] = 0xc3; // JMP
    emu->memory[1] = 0;    // 0x0100
    emu->memory[2] = 0x01;

    // fix the stack pointer from 0x6ad to 0x7ad
    // this 0x06 byte 112 in the code - which is
    // byte 112 + 0x100 = 368 in memory
    emu->memory[368] = 0x7;

    // skip DAA test
    emu->memory[0x59c] = 0xc3; // JMP
    emu->memory[0x59d] = 0xc2;
    emu->memory[0x59e] = 0x05;

    // some other initialization steps
    // we need the first thing to happen be jumping to the code at 0x100
    state->memory[0] = 0xc3; // JMP
    state->memory[1] = 0;    // 0x0100
    state->memory[2] = 0x01;

    // fix the stack pointer from 0x6ad to 0x7ad
    // this 0x06 byte 112 in the code - which is
    // byte 112 + 0x100 = 368 in memory
    state->memory[368] = 0x7;

    // skip DAA test
    state->memory[0x59c] = 0xc3; // JMP
    state->memory[0x59d] = 0xc2;
    state->memory[0x59e] = 0x05;

    // limit program speed
    struct timespec limit;
    clock_gettime(CLOCK_MONOTONIC, &limit);
    long curT = (limit.tv_sec * 1e9) + limit.tv_nsec; 
    long lastT = 0;

    // starting emulator
    float cycles = 0;
    state->pc = 0;
    emu->pc = 0;
    while (state->pc < 65536){

        // buffer for state comparison
        unsigned char *bufferEmu = (unsigned char *)malloc(sizeof(unsigned char) *198);
        unsigned char *bufferState = (unsigned char *)malloc(sizeof(unsigned char) *198);

        // test emu
        Disassemble(emu->memory, emu->pc);
        emulate_i8080(emu, &cycles);
        printConditionsEmu(emu, bufferEmu);
        

        if (emu->pc == 0x689){
            printf("something went wrong, exiting\n");
            exit(1);
        }


        // my emu
        Disassemble(state->memory, state->pc);
        Emulate8080Op(state, &cycles);
        printConditionsState(state, bufferState);
        
        if (state->pc == 0x689){
            printf("something went wrong, exiting\n");
            exit(1);
        }

        // compares both outputs
        compareRes(bufferEmu, bufferState);

        // resets buffer
        free(bufferEmu);
        free(bufferState);


/*
        // instruction flow controll
        while (curT - lastT < 200000000){
            clock_gettime(CLOCK_MONOTONIC, &limit);
            curT = (limit.tv_sec * 1e9) + limit.tv_nsec; 
        }
        lastT = (limit.tv_sec * 1e9) + limit.tv_nsec;
*/
        // error handling adress
        

        /*
        // current time to calculate interrupt timing
        clock_gettime(CLOCK_MONOTONIC, &start);
        long long curT = (start.tv_sec * 1e9) + start.tv_nsec; 
        
        // executes interrupts if timing allows
        if (cycles >= 16667)
            interProtocol(state, &rendHalf, &curT, &lastInterrupt, &start, renderer, &cycles);          
            */
    }
    
    free(state->memory);
    free(state);

    return 0;
}