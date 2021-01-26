/**
 * This file implements the code for the  CPU class of Chip8 emulator
 * 
 * @author Vincent Sun
 * @date 20/01/2021
 */
#include "chip.h"
#include <iostream>
#include <cstdlib>

// the fontset contains all the font of chip8
// size is 80 because there are 16 characters, each takes up 5pixels in height
unsigned char fontset[80] = {
                            0xF0, 0x90, 0x90, 0x90, 0xF0, //0
                            0x20, 0x60, 0x20, 0x20, 0x70, //1
                            0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
                            0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
                            0x90, 0x90, 0xF0, 0x10, 0x10, //4
                            0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
                            0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
                            0xF0, 0x10, 0x20, 0x40, 0x40, //7
                            0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
                            0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
                            0xF0, 0x90, 0xF0, 0x90, 0x90, //A
                            0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
                            0xF0, 0x80, 0x80, 0x80, 0xF0, //C
                            0xE0, 0x90, 0x90, 0x90, 0xE0, //D
                            0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
                            0xF0, 0x80, 0xF0, 0x80, 0x80  //F
                        };

/**
 * Constructor of cpu class
 */
chip::chip() {
    //Empty
}

/**
 * Destructor of cpu class
 */
chip::~chip() {
    //empty
}

void chip::init() {
    // Clear and Initialise the CPU's registers and memory
    sp = 0;
    ir = 0;
    opcode = 0;
    pc = 0x200; // The program counter starts at 0x200 as 0x000~0x1FF is reserved.

    // Initialise the V Registers to 0
    for (int i = 0; i < 16; i++) V[i] = 0;

    // Initialise the stack to 0
    for (int i = 0; i < 16; i++) stack[i] = 0;

    // Clear out the memory
    for (int i = 0; i < 4096; i++) memory[i] = 0;

    // Store the fontset in the memory
    for (int i = 0x50; i < 0xA0; i++) {
        memory[i] = fontset[i-0x50];
        // placing the unary "+" symbol before the hex values allows the ascii character to be printed as numbers
        // The statement std::hex prints all of the numbers in hex form
        //std::cout << "Location: " << std::hex << i  << ", " << +memory[i] << ", " << +fontset[i-0x50] << std::endl;
    }

    // Reset the timers
    delay_timer = 0;
    sound_timer = 0;

    // Clear the screen
    for (int i = 0; i < 2048; i++) screen[i] = 0;
    drawFlag = true;

    // Exit Flag
    exitFlag = false;

    // Seed Randomness
    std::srand(0);
}

bool chip::loadRom(const char* filename) {
    // Initialise the CPU before loading the ROM
    init();

    // Open the ROM file in binary format
    std::FILE* rom = std::fopen(filename, "rb");
    if (rom == NULL) {
        std::fputs("ROM is corrupted!\n", stderr);
        return false;
    }

    // Check if the ROM size can fit in the memory array
    std::fseek(rom, 0, SEEK_END); //seek to end of ROM
    long romSize = std::ftell(rom); //obtain the number of bytes from the beginning of this file
    //sets the file position indicator to the beginning of the given file stream
    std::rewind(rom);

    // Allocate memory to store the entire ROM
    char* buffer = (char*)std::malloc(sizeof(char)*romSize);
    if (buffer == NULL) {
        std::fputs("Memory Error\n", stderr);
        return false;
    }

    // Copy the ROM into the buffer
    size_t readSize = std::fread(buffer, 1, romSize, rom);
    if (readSize != romSize) {
        std::fputs("Reading Error\n", stderr);
        return false;
    }

    // Copy the buffer into Chip8's memory
    if (romSize < (4096 - 512)) { // pc starts at 0x200 (512), and memory is 4096bytes.
        for (int i = 0; i < romSize; i++) memory[i+pc] = buffer[i];
    }
    else std::cout << "ROM too big for memory" << std::endl;

    // Close the rom & free the buffer
    std::fclose(rom);
    std::free(buffer);

    return true;
}

void chip::oneCycle() {
    // Implementation of the 35 Chip8 opcodes
    // Read the current opcode
    opcode = (memory[pc] << 8) | memory[pc+1];
    //std::cout << "Current opcode is: " << std::hex << opcode << std::endl;
    int x = 0, y = 0;

    // Choose the relevant operation based on the opcode
    switch (opcode & 0xF000) {
        case 0x0000:
            // 2 opcode, 0x00E0, 0x00EE
            switch (opcode & 0x000F) {
                case 0x0000:
                    // 0x00E0, clear the screen
                    for (int i = 0; i < 2048; i++) screen[i] = 0;
                    drawFlag = true;
                    pc += 2;
                break;

                case 0x000E:
                    // 0x00EE, returns from a subroutine
                    sp--;           // decrease the stack pointer to prevent overwrite
                    pc = stack[sp]; // the address in the stack pointer is moved back to the program counter
                    pc += 2;        // increase program counter from the returned location
                break;

                default:
                    std::cout << "Unknown Opcode: " << std::hex << opcode << std::endl;
            }
        break;

        case 0x1000:
            // 1 opcode, 0x1NNN, jump to address NNN
            pc = opcode & 0x0FFF;
        break;

        case 0x2000:
            // 1 opcode, 0x2NNN, calls subroutine @ NNN
            stack[sp] = pc;         // store the current pc onto the stack
            sp++;                   // increment sp to prevent overwrite
            pc = opcode & 0x0FFF;   // move to the the address
        break;

        case 0x3000:
            // 1 opcode, 0x3XNN, Skips the next instruction if Vx == NN
            x = (opcode & 0x0F00) >> 8;
            if (V[x] == (opcode & 0x00FF)) pc += 4;
            else pc += 2;
        break;

        case 0x4000:
            // 1 opcode, 0x4XNN, Skips the next instruction if Vx != NN
            x = (opcode & 0x0F00) >> 8;
            if (V[x] != (opcode & 0x00FF)) pc += 4;
            else pc += 2;
        break;

        case 0x5000:
            // 1 opcode, 0x5XY0, skips the next instruction if Vx == Vy
            x = (opcode & 0x0F00) >> 8;
            y = (opcode & 0x00F0) >> 4;
            if (V[x] == V[y]) pc += 4;
            else pc += 2;
        break;

        case 0x6000:
            // 1 opcode, 0x6XNN, Set Vx to NN
            x = (opcode & 0x0F00) >> 8;
            V[x] = opcode & 0x00FF;
            pc += 2;
        break;

        case 0x7000:
            // 1 opcode, 0x7XNN, add NN to Vx (Vx += NN)
            x = (opcode & 0x0F00) >> 8;
            V[x] += opcode & 0x0FF;
            pc += 2;
        break;
        
        case 0x8000:
            // 9 opcodes, 0x8XY0, 0x8XY1, 0x8XY2, 0x8XY3, 0x8XY4, 0x8XY5
            // 0x8XY6, 0x8XY7, 0x8XYE
            x = (opcode & 0x0F00) >> 8;
            y = (opcode & 0x00F0) >> 4;
            switch (opcode & 0x000F) {
                case 0x0000:
                    // 0x8XY0, Set Vx to the value of Vy (Vx = Vy)
                    V[x] = V[y];
                    pc += 2;
                break;

                case 0x0001:
                    // 0x8XY1, Set Vx to Vx bitwise or Vy (Vx |= Vy)
                    V[x] |= V[y];
                    pc += 2;
                break;

                case 0x0002:
                    // 0x8XY2, Set Vx to Vx bitwise and Vy (Vx &= Vy)
                    V[x] &= V[y];
                    pc += 2;
                break;

                case 0x0003:
                    // 0x8XY3, Set Vx to Vx XOR Vy (Vx ^= Vy)
                    V[x] ^= V[y];
                    pc += 2;
                break;

                case 0x0004:
                    // 0x8XY4, Add Vy to Vx (Vx += Vy). If there is a carry, Vf=1 o/w 0
                    V[0xF] = (V[y] > (0xFF - V[x])) ? 1:0; // check if there will be carry (only happens when Vy > (0xFF - Vx))
                    V[x] += V[y];
                    pc += 2;
                break;

                case 0x0005:
                    // 0x8XY5, Vy is subtracted from Vx (Vx -= Vy). If there is a burrow, Vf=0 o/w 1
                    V[0xF] = (V[y] > V[x]) ? 0:1; // Check if there will be a burrow (happens when Vy > Vx)
                    V[x] -= V[y];
                    pc += 2;
                break;

                case 0x0006:
                    // 0x8XY6, Store the least significant bit of Vx in Vf and then shifts Vx to the right by 1
                    V[0xF] = V[x] & 0x1;
                    V[x] >>= 1;
                    pc += 2;
                break;

                case 0x0007:
                    // 0x8XY7, Set Vx to Vy - Vx (Vx = Vy - Vx). Vf=0 if there's a burrow o/w 1
                    V[0xF] = (V[x] > V[y]) ? 0:1; // Check for burrow (happens when Vx > Vy)
                    V[x] = V[y] - V[x];
                    pc += 2;
                break;

                case 0x000E:
                    // 0x8XYE, Stores the most significant bit of Vx in Vf then shift Vx to the left by 1
                    V[0xF] = (V[x] & 0xF000) >> 7; // the register contains 8-bit, so right shift by 7 bits will give us the MSB
                    V[x] <<= 1;
                    pc += 2;
                break;

                default:
                    std::cout << "Unknown Opcode: " << std::hex << opcode << std::endl;
            }
        break;

        case 0x9000:
            // 1 opcode, 0x9XY0, Skips the next instruction if Vx != Vy
            x = (opcode & 0x0F00) >> 8;
            y = (opcode & 0x00F0) >> 4;
            if (V[x] != V[y]) pc += 4;
            else pc += 2;
        break;

        case 0xA000:
            // 1 opcode, 0xANNN, Set ir to the address NNN
            ir = opcode & 0x0FFF;
            pc += 2;
        break;

        case 0xB000:
            // 1 opcode, 0xBNNN, Jump to the address NNN + V0 (pc = NNN+V0)
            pc = (opcode & 0x0FFF) + V[0x0];
        break;

        case 0xC000:
            // 1 opcode, 0xCXNN, Set Vx to the result of a bitwise and operation on a random number (0~255) and NN (Vx=rand()&NN)
            x = (opcode & 0x0F00) >> 8;
            V[x] = (rand()%0xFF) & (opcode & 0x00FF);
            pc += 2;
        break;

        case 0xD000: {
            // 1 opcode, 0xDXYN, Draw a sprite at coordinate (Vx, Vy) with width of 8pixels and height of N pixels
            // Each row of 8 pixels is read as bit-code starting from memory location ir. ir's value doesn't change
            // after the execution of this instruction.
            // Vf is set to 1 if any screen pixel is flipped from set to unset when sprite is drawn.
            // 0 if that doesn't happen
            int vx = (opcode & 0x0F00) >> 8;
            int vy = (opcode & 0x00F0) >> 4;
            unsigned short x_col = V[vx];
            unsigned short y_row = V[vy];
            unsigned short height = opcode & 0x000F;
            unsigned short pixel;

            V[0xF] = 0;
            for (int row = 0; row < height; row++) {
                pixel = memory[ir + row]; // Fetch pixel value from memory starting @ location ir
                for (int col = 0; col < 8; col++) {
                    if ((pixel & (0x80 >> col)) != 0) { // Check if current pixel is set to 1 (0x80==1000 0000, so by shifting over by col, we can check if it's 1, one bit at a time)
                        if (screen[(x_col + col + ((y_row+row)*64))] == 1) V[0xF] = 1; // check for collision (if the current pixel screen is already 1)
                        screen[(x_col + col + ((y_row+row)*64))] ^= 1;
                    }
                }
            }

            drawFlag = true;
            pc += 2;
        break;}

        case 0xE000:
            // 2 opcode possibility, 0xEX9E, 0xEXA1
            switch (opcode & 0x00F0) {
                x = (opcode & 0x0F00) >> 8;
                case 0x0090:
                    // 0xEX9E. Skips the next instruction if key stored in Vx is pressed (key()==Vx)
                    if (key[V[x]] != 0) pc += 4;
                    else pc += 2;
                break;

                case 0x00A0:
                    // 0xEXA1. Skips the next instruction if key stored in Vx isn't pressed (key()!=Vx)
                    if (key[V[x]] == 0) pc += 4;
                    else pc += 2;
                break;

                default:
                    std::cout << "Unknown Opcode: " << std::hex << opcode << std::endl;
            }
        break;

        case 0xF000:
            // 9 opcode, 0xFX07, 0xFX0A, 0xFX15, 0xFX18, 0xFX1E, 0xFX29
            // 0xFX33, 0xFX55, 0xFX65
            switch (opcode & 0x00FF) {
                x = (opcode & 0x0F00) >> 8;
                case 0x0007:
                    // 0xFX07. Set Vx to the value of delay timer (Vx = delay_timer)
                    V[x] = delay_timer;
                    pc += 2;
                break;

                case 0x000A: {
                    // 0xFX0A. A key press is awaited and then stored in Vx (Blocking Operation). (Vx=keyDown())
                    bool pressed = false;

                    for (int i = 0; i < 16; i++) {
                        if (key[i] != 0) {
                            V[x] = i;
                            pressed = true;
                        }
                    }
                    if (!pressed) return;
                    pc += 2;
                break;}

                case 0x0015:
                    // 0xFX15. Set delay_timer to Vx (Vx = delay_timer)
                    delay_timer = V[x];
                    pc += 2;
                break;

                case 0x0018:
                    // 0xFX18. Set sound_timer to Vx (Vx = sound_timer)
                    sound_timer = V[x];
                    pc += 2;
                break;

                case 0x001E:
                    // 0xFX1E. Add Vx to ir. Vf is not affected. (ir += Vx)
                    ir += V[x];
                    pc += 2;
                break;

                case 0x0029:
                    // 0xFX29. Sets ir to the location of the sprite for the character in Vx.
                    ir = V[x]*0x5; // the sprite is 5pixels wide
                    pc += 2;
                break;

                case 0x0033:
                    // 0xFX33. Stores the binary-coded decimal representation of Vx.
                    // Set teh most significant of the three digits in ir, middle digit in ir+1, least significant in ir+2
                    // ie, take the decimal representaion of Vx
                    // hundreds digit is stored in memory location ir
                    // tens digit in memory location ir+1
                    // ones digit in memory location ir+2
                    memory[ir] = V[x]/100;
                    memory[ir+1] = (V[x]/10)%10;
                    memory[ir+2] = (V[x]%100)%10;
                    pc += 2; 
                break;

                case 0x0055:
                    // 0xFX55. Stores V0 to Vx (including Vx) in memory starting @ address ir.
                    // The offset from ir is increased by 1 for each value written, but ir is left unmodified
                    for (int i = 0; i <= x; i++) memory[ir+i] = V[i];
                    ir += x+1;
                    pc += 2;
                break;

                case 0x0065:
                    // 0xFX66. Fills V0 to Vx (including Vx) in memory starting @ address ir.
                    // The offset from ir is increased by 1 for each value written, but ir itself is left unmodified
                    for (int i = 0; i <= x; i++) V[i] = memory[ir+i];
                    ir += x+1;
                    pc += 2;
                break;

                default:
                    std::cout << "Unknown Opcode: " << std::hex << opcode << std::endl;
            }
        break;

        default:
            std::cout << "Unknown Opcode: " << std::hex << opcode << std::endl;
    }

    // Update timers
    if (delay_timer > 0) delay_timer--;
    if (sound_timer > 0) {
        if (sound_timer == 1) std::cout << "Buzz Buzz" << std::endl;
        sound_timer--;
    }
}

void chip::keyDown(unsigned char inputKey) {
    switch (inputKey) {
        case 27:
            exitFlag = true;
        break;
        case 0x1:
            key[0x1] = 1;
        break;
        case 0x2:
            key[0x2] = 1;
        break;
        case 0x3:
            key[0x3] = 1;
        break;
        case 0xC:
            key[0xC] = 1;
        break;
        case 0x4:
            key[0x4] = 1;
        break;
        case 0x5:
            key[0x5] = 1;
        break;
        case 0x6:
            key[0x6] = 1;
        break;
        case 0xD:
            key[0xD] = 1;
        break;
        case 0x7:
            key[0x7] = 1;
        break;
        case 0x8:
            key[0x8] = 1;
        break;
        case 0x9:
            key[0x9] = 1;
        break;
        case 0xE:
            key[0xE] = 1;
        break;
        case 0xA:
            key[0xA] = 1;
        break;
        case 0x0:
            key[0x0] = 1;
        break;
        case 0xB:
            key[0xB] = 1;
        break;
        case 0xF:
            key[0xF] = 1;
        break;
        default:
            std::cout << "Unrecognised Key!" << std::endl;
    }
}

void chip::keyUp(unsigned char inputKey) {
    switch (inputKey) {
        case 0x1:
            key[0x1] = 0;
        break;
        case 0x2:
            key[0x2] = 0;
        break;
        case 0x3:
            key[0x3] = 0;
        break;
        case 0xC:
            key[0xC] = 0;
        break;
        case 0x4:
            key[0x4] = 0;
        break;
        case 0x5:
            key[0x5] = 0;
        break;
        case 0x6:
            key[0x6] = 0;
        break;
        case 0xD:
            key[0xD] = 0;
        break;
        case 0x7:
            key[0x7] = 0;
        break;
        case 0x8:
            key[0x8] = 0;
        break;
        case 0x9:
            key[0x9] = 0;
        break;
        case 0xE:
            key[0xE] = 0;
        break;
        case 0xA:
            key[0xA] = 0;
        break;
        case 0x0:
            key[0x0] = 0;
        break;
        case 0xB:
            key[0xB] = 0;
        break;
        case 0xF:
            key[0xF] = 0;
        break;
        default:
            std::cout << "Unrecognised Key!" << std::endl;
    }
}