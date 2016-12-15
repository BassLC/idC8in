#include <iostream>
#include <string>
#include <fstream>
#include "SDL2/SDL.h"
#include <array>

class Chip8 {

public:
  Chip8();
  ~Chip8();
  bool load_rom(const char *ROM);
  void decrease_timers();
  void decode_opcode();
  
private:
  std::array<uint8_t, 4096> memory;
  uint16_t opcode;
  std::array<uint8_t, 16> registers;
  uint16_t index, program_counter;
  std::array<uint16_t, 16> stack;

  const char beep = '\a';
  std::array<uint8_t, 16> keyboard;
  uint8_t timer_delay, timer_sound;
  
  const int WINDOW_HEIGHT = 64;
  const int WINDOW_WIDTH = 32;
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
    
};


Chip8::Chip8(){
  //Clearing and initializing everything
  opcode = 0;
  program_counter = 0x200;
  index = 0;
  program_counter = 0;
  timer_delay = 0;
  timer_sound = 0;
  
  registers.fill(0);
  memory.fill(0);
  stack.fill(0);
  keyboard.fill(0);
}


Chip8::~Chip8(){}


bool Chip8::load_rom(const char *ROM_location) {
  std::ifstream ROM (ROM_location);

  if ( ROM.is_open() ) {
    ROM.close();
    return true;
  }

  std::cerr << "Can't Read File.\n";
  return false;
}


void Chip8::decode_opcode() {
  //TODO
}


void Chip8::decrease_timers() {
  if (timer_delay > 0) --timer_delay;
  if (timer_sound > 0) --timer_sound;
}
