#include "IO.h"

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