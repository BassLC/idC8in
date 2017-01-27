#OBJS specifies which files to compile as part of the project
#OBJS=main

#CC specifies which compiler we're using
CC=clang++

#C++ version
VV=--std=c++14

# WARNINGS specifies all the warnings
WARNINGS=-Wall -Wextra -Wpedantic

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS=-lSDL2 -o

#This is the target that compiles our executable
cpu:
	$(CC) $(VV) $(WARNINGS) cpu.cpp $(LINKER_FLAGS) cpu
debug:
	$(CC) $(VV) $(WARNINGS) main.cpp cpu.cpp -g $(LINKER_FLAGS) idC8in
all: 
	$(CC) $(VV) $(WARNINGS) main.cpp cpu.cpp -O3 $(LINKER_FLAGS) idC8in
