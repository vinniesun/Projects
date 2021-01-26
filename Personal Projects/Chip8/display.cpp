/**
 * This file implements the code for the  CPU class of Chip8 emulator
 * To test this code works, use the command: clangg++ -std=c++11 display.cpp -I include -L lib -l SDL2 -o display
 * 
 * @author Vincent Sun
 * @date 20/01/2021
 */

#include "display.h"

Display::Display() {
    window = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, (DISPLAY_WIDTH*SCALE), (DISPLAY_HEIGHT*SCALE), SDL_WINDOW_SHOWN);
    if (!window) std::cout << "window initialisation error, error: " << SDL_GetError() << std::endl;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) std::cout << "renderer initialisation error, error: " << SDL_GetError() << std::endl;
    SDL_RenderSetScale(renderer, SCALE, SCALE);
}

Display::~Display() {
    if (window) SDL_DestroyWindow(window);
    if (renderer) SDL_DestroyRenderer(renderer);
}

void Display::update(chip &chip8) {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    for (int row = 0; row < DISPLAY_HEIGHT; row++) {
        for (int col = 0; col < DISPLAY_WIDTH; col++) {
            if (chip8.screen[(row*DISPLAY_WIDTH) + col] != 0) SDL_RenderDrawPoint(renderer, col, row);
        }
    }
    SDL_RenderPresent(renderer);
    chip8.drawFlag = false;
}
