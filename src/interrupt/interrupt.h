#include <stdbool.h>
#include <SDL2/SDL.h>
#include <time.h>

#include "../processor/processor.h"
#include "../graphics/render.h"

void GenerateInterrupt(State8080* state, int interrupt_num);

// CPU clock rate = 2MHz; 1 clock per 500 ns; 1 frame per 16666667 ns; half frame per 8333334 ns
// time per frames rendered / CPU time per clock = (16666667 ns) / (500 ns) = 33334 cycles;  divider by 2 = 16667 cycles per half frame
void interProtocol(State8080 * state, bool *rendHalf, long long *curT, long long *lastInterrupt, struct timespec *start,SDL_Renderer *renderer, float *cycles);