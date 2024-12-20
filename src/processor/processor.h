#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "../../Disassembler/disDebug.h"

#ifndef IO_H
#define IO_H

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

#endif // IO_H

State8080 *init8080();

void UnimplementedInstruction(State8080 *state);

int Emulate8080Op(State8080 *state, float * cycles);

int parity(uint8_t b);
