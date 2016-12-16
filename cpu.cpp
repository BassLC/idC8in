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

  //Font loading
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
    for (std::size_t i = 0x200; i < 4096; ++i) {
      program_counter = i;
      decode_opcode();

      
    }
  }

  decrease_timers();
}


void Chip8::decode_opcode() {
  //Fetch opcode
  opcode = memory[program_counter] << 8 | memory[program_counter + 1]; 

  //std::cout << std::hex << (uint16_t)(opcode) << std::endl;
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
    //std::cout << "Could not read OPCODE: " << std::hex << (uint16_t)opcode << std::endl;
    break;
  }
  
}



void Chip8::decrease_timers() {
  if (timer_delay > 0) --timer_delay;
  if (timer_sound > 0) --timer_sound;
}

int main(int argc, char *argv[]) {
  Chip8 chip8;

  if ( argc != 2 ) {
    std::cout << "Usage: idc8in 'ROM_LOCATION'" << std::endl;
    return 1;
  }
  
  chip8.load_rom(argv[1]);
  chip8.cycle();
  return 0;
}
