#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <inttypes.h>
#include <stdint.h>
#include <string.h>

// ref: http://www.emulator101.com/emulator-shell.html

// set up struct for CPU condition codes
typedef struct ConditionFlags
{
    uint8_t z : 1;
    uint8_t s : 1;
    uint8_t p : 1;
    uint8_t cy : 1;
    uint8_t ac : 1;
    uint8_t pad : 3;
} ConditionFlags;

// set up struct for CPU state
typedef struct emu8080
{
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t sp;
    uint16_t pc;
    uint8_t *memory;
    struct ConditionFlags cc;
    uint8_t int_enable;
} emu8080;

// quit the program for every opcode with an error
void unimplemented_instruction(emu8080 *state);

// emulate the opcode given the current CPU state
int emulate_i8080(emu8080 *state, float *cycles);

// set flags after arithmetic function on A register
void arithmetic_flags_A(emu8080 *state, int answer);

// set flags after logical instruction on A register
void logic_flags_A(emu8080 *state);

int parityEmu(int x, int size);

void redirect_output(uint8_t byte, uint8_t port);

void printConditionsEmu(emu8080 *emu, unsigned char *buf);


#endif /* PROCESSOR_H */