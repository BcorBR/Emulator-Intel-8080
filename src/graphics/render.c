#include "render.h"

void render(State8080 *state, bool rendHalf, SDL_Renderer *renderer){
    // used to analyze individual bit
    int bit;
    
    // 224x256
    // stops at line 96 one px before center px of the screen, mem 2b7b
    if (rendHalf){
        int Px = 0, Py = 0, mem = 0x2400, shift;
        
        for (int i = 0; i < 32 * 96; i++){
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
            mem++;
        }
    }
    else{
        int Px = 0, Py = 96, mem = 0x3000, shift;
        for (int i = 0; i <= 32 * 128; i++){
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
            mem++;
        }
    }
    SDL_RenderPresent(renderer);
}