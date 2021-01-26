/**
 * The header file for the display class of Chip8 emulator
 * 
 * @author Vincent Sun
 * @date 20/01/2021
 */

/**
 * Display Class implementation
 */
#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include "chip.h"

class Display {
private:
    static const uint8_t DISPLAY_WIDTH = 64;
    static const uint8_t DISPLAY_HEIGHT = 32;
    static const uint8_t SCALE = 10;
    SDL_Window* window;
    SDL_Renderer* renderer;

public:
    Display();      // Constructor
    ~Display();     // Destructor
    void update(chip &chip8);  // Update the current screen based on the status of Chip8
};