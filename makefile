#OBJS specifies which files to compile as part of the project
OBJS=main

#CC specifies which compiler we're using
CC=clang++

#C++ version
VV=--std=c++14

# WARNINGS specifies all the warnings
WARNINGS=-Wall -Wextra -Wpedantic

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS=-lSDL2 -o

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(VV) $(WARNINGS) $(OBJS).cpp $(LINKER_FLAGS) $(OBJS)
