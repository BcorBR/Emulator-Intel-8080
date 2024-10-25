#include "../Disassembler/disDebug.h"
#include "./FileHandle/file.h"
#include "./interrupt/interrupt.h"


int main(int argc, char *argv[]){
    // reads ROM into buffer
    buffer *buf = file(argc, argv);

    // initializes processor emulator
    State8080 *state = init8080(state);

    // writing ROM into memory
    for (int i = 0; i < buf->size; i ++)
        state->memory[i] = buf->data[i];
    
    free(buf);

    // initialize SDL renderer
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0){
        printf("SDL could not initialize: %s\n", SDL_GetError());
        exit(1);
    }
    if (SDL_CreateWindowAndRenderer(256, 224, 0, &window, &renderer) < 0){
        printf("WIndow and renderer could not be created: %s\n", SDL_GetError());
        exit(1);
    }
    
    // will be used to choose between first and last half of screen
    // to be rendered, 1 = upper screen, 0 = lower screen
    bool rendHalf = true;

    // initializing interrupt time 
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    long long lastInterrupt = 0;

    // starting emulator
    float cycles = 0;
    state->pc = 0;
    while (state->pc < 65536){
        Disassemble(state->memory, state->pc);
        Emulate8080Op(state, &cycles);
        
        // current time to calculate interrupt timing
        clock_gettime(CLOCK_MONOTONIC, &start);
        long long curT = (start.tv_sec * 1e9) + start.tv_nsec; 
        
        // executes interrupts if enough cycles
        if (cycles >= 16667)
            interProtocol(state, &rendHalf, &curT, &lastInterrupt, &start, renderer, &cycles);          
    }
    
    free(state->memory);
    free(state);

    return 0;
}