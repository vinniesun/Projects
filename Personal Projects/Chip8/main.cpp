/**
 * Compile with the command:
 * clang++ -std=c++11 main.cpp cpu.cpp -o main
 * 
 * test the code with the command
 * ./main (rom_name)
 */
#include "chip.h"
#include "display.h"
#include <unordered_map>

chip chip8;

int main(int argc, char** argv) {
    // Check if the ROM is provided
    if (argc < 2) {
        std::cout << "Input ROM isn't specified!" << std::endl;
        return -1;
    }

    // Check if SDL initialised successfully
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL Initialisation Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    Display myDisplay;

    if (!chip8.loadRom(argv[1])) {
        std::cout << "Failed to load ROM!" << std::endl;
        return -1;
    }
    
    std::unordered_map<SDL_Keycode, unsigned char> keymap;
    keymap[SDLK_1] = 0x1;
    keymap[SDLK_2] = 0x2;
    keymap[SDLK_3] = 0x3;
    keymap[SDLK_4] = 0xC;
    keymap[SDLK_q] = 0x4;
    keymap[SDLK_w] = 0x5;
    keymap[SDLK_e] = 0x6;
    keymap[SDLK_r] = 0xD;
    keymap[SDLK_a] = 0x7;
    keymap[SDLK_s] = 0x8;
    keymap[SDLK_d] = 0x9;
    keymap[SDLK_f] = 0xE;
    keymap[SDLK_z] = 0xA;
    keymap[SDLK_x] = 0x0;
    keymap[SDLK_c] = 0xB;
    keymap[SDLK_v] = 0xF;

    SDL_Event e;
    bool run = true;

    uint32_t last_tick = SDL_GetTicks();

    while (run) {
        uint32_t start_tick = SDL_GetTicks();

        while (SDL_PollEvent(&e)) {
            const auto key = e.key.keysym.sym;
            switch (e.type) {
                case SDL_QUIT:
                    run = false;
                break;

                case SDL_KEYDOWN: {
                    if (key == SDLK_ESCAPE) run = false;
                    if (keymap.find(key) != keymap.end()) chip8.keyDown(keymap[key]);
                }
                break;

                case SDL_KEYUP: {
                    if (key == SDLK_ESCAPE) run = false;
                    if (keymap.find(key) != keymap.end()) chip8.keyUp(keymap[key]);
                }
                break;
            }
        }

        // For 60FPS/60Hz, the period should be 1/60=0.0167s, which is 16.7ms
        // One tick in SDL is 1ms, therefore to achieve 60FPS, the tick time should
        // be greater than 16.
        if (start_tick - last_tick > 16) {
            chip8.oneCycle();
        }

        if (chip8.drawFlag) {
            myDisplay.update(chip8);
            last_tick = start_tick;
        }
    }

    SDL_Quit();

    return 0;
}