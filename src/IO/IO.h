#include <SDL2/SDL.h>

#include "../processor/processor.h"

uint8_t MachineIN(State8080 *state, uint8_t port);

void MachineOUT(State8080 *state, uint8_t port);