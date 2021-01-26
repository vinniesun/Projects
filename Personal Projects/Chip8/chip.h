/**
 * The header file for the chip class of Chip8 emulator
 * 
 * @author Vincent Sun
 * @date 20/01/2021
 */

/**
 * chip Class implementation
 */
#pragma once

class chip {
private:
    unsigned short pc; // Program Counter
    unsigned short sp; // Stack Pointer
    unsigned short ir; // Index Register
    unsigned short opcode; // Current Opcode

    unsigned char delay_timer; // Delay Timer @ 60Hz
    unsigned char sound_timer; // Sound Timer @ 60Hz

    unsigned char V[16]; // The 16 V Register
    unsigned char memory[4096]; // The memory map location for Chip8
    unsigned short stack[16]; // The system contains 16 levels of stack

    bool exitFlag;

    void init();

public:
    bool drawFlag; // Flag to determine whether the screen need to be updated
    unsigned char screen[64*32]; // An array used to store the bits representing the screen, total of 2048 pixels
    unsigned char key[16]; // key ranges from 0~F (in hex)

    chip(); // Constructor
    ~chip(); // Destructor

    bool loadRom(const char* filename); // Load the ROM for emulation
    void oneCycle(); // Emulate one instruction cycle of Chip8.
    void keyDown(unsigned char inputKey); // Check if the particular key is pressed
    void keyUp(unsigned char inputKey); // Check if the particular key is released
};