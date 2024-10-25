#include "interrupt.h"

void GenerateInterrupt(State8080* state, int interrupt_num){
    state->memory[state->sp - 1] = ((state->pc+1) >> 8) & 0b11111111;
    state->memory[state->sp - 2] =  (state->pc+1) & 0b11111111;
    state->sp -= 2;
    
    // mimic "DI" - disable interrupt
    // This should prevent a new interrupt from \
       generating until EI (enable interrupt) is called.
    state->int_enable = 0;
    
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
    
    // only do an interrupt if they are enabled    
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