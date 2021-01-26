# This is the makefile for Chip8
# The Compiler: g++
CXX = clang++

# Compiler Flag
CXXFLAGS_o = -std=c++11 -Wall -I include -L lib -l SDL2
CXXFLAGS = -std=c++11 -Wall -I include -L lib -l SDL2
# Dependencies
DEPS = chip.h display.h
# Objects
OBJ = chip.o display.o main.o

%.o: %.c $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS_o)

main: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -f *.o