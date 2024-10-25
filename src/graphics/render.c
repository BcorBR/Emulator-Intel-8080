#include "render.h"

void render(State8080 *state, bool rendHalf, SDL_Renderer *renderer){
    int Px = 0, Py = 0, shift;
    // used to analyze individual bit
    int bit;
    
    // 256x224
    // stops at line 96 one px before center px of the screen
    if (rendHalf){
        for (int mem = 0x2400; mem <= 0x2b7b; mem++){
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