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
  void cycle();
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
  timer_delay = 0;
  timer_sound = 0;
  
  registers.fill(0);
  memory.fill(0);
  stack.fill(0);
  
  keyboard = {
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v,
  };

  const std::array<uint8_t, 80> fontset = { 
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  for ( std::size_t i = 0; i < 80; ++i ) {memory[i] = fontset[i];}
}


Chip8::~Chip8(){}


bool Chip8::load_rom(const char *ROM_location) {
  std::ifstream ROM (ROM_location, std::ios::binary | std::ios::ate);
  
  if ( ROM.is_open() ) {
    const auto size = ROM.tellg();
    ROM.seekg(std::ios::beg);
    
    if ( size < 3584 ) { //4096 - 0x200 = 3584 
      ROM.read((char*)&memory[0x200], size);
      std::cout << "ROM loaded!\n";
      return true;
      
    } else {
      std::cerr << "ROM too big!\n";
      ROM.close();
      return false;
    }
  }
  
  std::cerr << "Can't Read File.\n";
  return false;
}


void Chip8::cycle() {
  if ( timer_delay == 0 ) {
    decode_opcode();
  }
  decrease_timers();
}


void Chip8::decode_opcode() {
  //Fetch opcode
  opcode = memory[program_counter] << 8 | memory[program_counter + 1]; 

  switch ( opcode & 0xF000 ) {

  case 0x00E0: //Opcode to clear screen
    std::cout << "CLR\n";
    break;

  case 0x00EE: //Returns subroutine
    std::cout << "RETSUB\n";
    break;
    
  case 0x1000:
    program_counter = memory[opcode & 0b1111];
    std::cout << "JMP\n";
    break;
    
  default:
    std::cout << "Could not read OPCODE: " << std::hex << opcode << ", "<< program_counter << std::endl;
    break;
  }
  
}



void Chip8::decrease_timers() {
  if (timer_delay > 0) --timer_delay;
  if (timer_sound > 0) --timer_sound;
}

int main(int argc, char *argv[]) {
  if ( argc != 2 ) {
    std::cout << "Usage: idc8in 'ROM_LOCATION'" << std::endl;
    return 1;
  }
  
  Chip8 chip8;
  chip8.load_rom(argv[1]);
  chip8.cycle();
  return 0;
}
