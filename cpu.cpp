#include <iostream>
#include <string>
#include <fstream>
#include <array>
#include <random>
#include "SDL2/SDL.h"


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
  uint16_t index, program_counter, stack_pointer;
  std::array<uint16_t, 16> stack;

  const char beep = '\a';
  std::array<uint8_t, 16> keyboard;
  uint8_t timer_delay, timer_sound;
  
  // const int WINDOW_HEIGHT = 64;
  // const int WINDOW_WIDTH = 32;
  // SDL_Window* window = nullptr;
  // SDL_Renderer* renderer = nullptr;
};


Chip8::Chip8(){
  //Clearing and initializing everything
  opcode = 0;
  program_counter = 0x200;
  index = 0;
  timer_delay = 0;
  timer_sound = 0;
  stack_pointer = 0;
  
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

  for ( std::size_t i = 0; i < 80; ++i ) { memory[i] = fontset[i]; }
}


Chip8::~Chip8(){}


bool Chip8::load_rom(const char *ROM_location) {
  std::ifstream ROM (ROM_location, std::ios::binary | std::ios::ate);
  
  if ( ROM.is_open() ) {
    const auto size = ROM.tellg();
    ROM.seekg(std::ios::beg);
    
    if ( size < 3584 && size > 0 ) { //4096 - 0x200 = 3584 
      ROM.read((char*)&memory[0x200], size);
      std::cout << "ROM loaded!\n";
      return true;
      
    } else {
      std::cerr << "ROM too big!\n";
      return false;
    }
  }
  ROM.close();
  std::cerr << "Can't Read File.\n";
  return false;
}


void Chip8::cycle() {
  while ( program_counter < 4095 ) {
    //std::cout << program_counter << '\n';
    if ( timer_sound > 0 ) {
      std::cout << beep; //Using terminal specific interactions (BURRFIX)
    }

    if ( timer_delay == 0 ) {
      decode_opcode();
    }
  
    decrease_timers();  
  }
}


void Chip8::decode_opcode() {
  //Fetch opcode
  opcode = memory[program_counter] << 8 | memory[program_counter + 1]; 
  
  //Random number generator
  const auto rand_numb = [](){
    const auto seed = std::random_device()();
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> ui(0, 255);
    return (uint8_t)ui(generator);
  };
  
  switch ( opcode & 0xF000 ) {
    
  case 0x0000:
    switch ( opcode & 0x0FFF ) {
    case 0xE0:
      std::cout << "NOT DONE: CLR\n";
      break;
    case 0xEE:
      std::cout << "NOT DONE: RET\n";
      break;
    }
    break;

  case 0x1000:
    std::cout << "JMP1\n";
    program_counter = opcode & 0x0FFF;
    return;
    
  case 0x2000:
    std::cout << "CALL2\n";
    ++stack_pointer;
    stack[stack_pointer] = program_counter;
    program_counter = opcode & 0x0FFF;
    return;

  case 0x3000:
    std::cout << "EQUAL REG BYTE\n";
    if ( registers[opcode & 0x0F00] == (opcode & 0x00FF) ) {
      program_counter += 2;
    }
    break;

  case 0x4000:
    std::cout << "NOT EQUAL REG4 BYTE\n";
    if ( !(registers[opcode & 0x0F00] == (opcode & 0x00FF)) ) {
      program_counter += 2;
    }
    break;
    
  case 0x5000:
    std::cout << "EQUAL REG1 REG2\n";
    if ( registers[opcode & 0x0F00] == registers[opcode & 0x00F0] ) {
      program_counter += 2;
    }
    break;

  case 0x6000:
    std::cout << "LOAD REG BYTE\n";
    registers[opcode & 0x0F00] = opcode & 0x00FF;
    break;
    
  case 0x7000:
    std::cout << "ADD REG BYTE\n";
    registers[opcode & 0x0F00] += opcode & 0x00FF;
    break;

  case 0x8000:
    switch ( opcode & 0x000F ) {

    case 0:
      std::cout << "LOAD REG1 REG2\n";
      registers[opcode & 0x0F00] = registers[opcode & 0x00F0];
      break;
      
    case 1:
      std::cout << "OR REG1 REG2\n";
      registers[opcode & 0x0F00] |= registers[opcode & 0x00F0]; 
      break;

    case 2:
      std::cout << "AND REG1 REG2\n";
      registers[opcode & 0x0F00] &= registers[opcode & 0x00F0]; 
      break;
      
    case 3:
      std::cout << "XOR REG1 REG2\n";
      registers[opcode & 0x0F00] ^= registers[opcode & 0x00F0]; 
      break;

    case 4:
      std::cout << "ADD REG1 REG2\n";
      if ( (uint16_t)(registers[opcode & 0x0F00] + registers[opcode & 0x00F0]) > 255 ) {
        registers[0xF] = 1;
      }
      registers[opcode & 0x0F00] += registers[opcode & 0x00F0];
      break;

    case 5:
      std::cout << "SUB REG1 REG2\n";
      if ( registers[opcode & 0x0F00] > registers[opcode & 0x00F0] ) {
        registers[0xF] = 1;
      } else {
        registers[0xF] = 0;
      }
      registers[opcode & 0x0F00] -= registers[opcode & 0x00F0];
      break;

    case 6:
      std::cout << "SHR REG1 REG2\n";
      registers[0xF] = registers[opcode & 0x0F00] & 0x1;
      registers[opcode & 0x0F00] >>= 2;
      break;

    case 7:
      std::cout << "SUBN REG1 REG2\n";
      if ( registers[opcode & 0x0F00] < registers[opcode & 0x00F0] ) {
        registers[0xF] = 1;
      } else {
        registers[0xF] = 0;
      }
      registers[opcode & 0x0F00] = registers[opcode & 0x00F0] - registers[opcode & 0x0F00];
      break;

    case 0xE:
      std::cout << "SHL REG1 REG2\n";
      registers[0xF] = registers[opcode & 0x0F00] & 0x80; 
      registers[opcode & 0x0F00] <<= 1;
      break;
    }
    break;

  case 0x9000:
    std::cout << "NOT EQUAL REG1 REG2 SKIP\n";
    if ( registers[opcode & 0x0F00] != registers[opcode & 0x00F0] ) {
      program_counter += 2;
    }
    break;

  case 0xA000:
    std::cout << "SET INDEX BYTE\n";
    index = opcode & 0x0FFF;
    break;

  case 0xB000:
    std::cout << "JMP ADDR+REG[0]\n";
    program_counter = (opcode & 0x0FFF) + registers[0];
    return;
    
  case 0xC000:
    std::cout << "SET REG1 (RANDOM AND BYTE)\n";
    registers[opcode & 0x0F00] = rand_numb() & (opcode & 0x00FF);  
    break;

  case 0xD000:
    std::cout << "NOT DONE DRW???\n";
    break;

  case 0xE000:
    switch ( opcode & 0xFF )  {
      
    case 0x9E:
      std::cout << "NOT DONE SKIP IF REG1 IS PRESSED\n";
      break;

    case 0xA1:
      std::cout << "NOT DONE SKIP IF REG1 IS NOT PRESSED\n";
      break;
    }
    break;

  case 0xF000:
    switch ( opcode & 0xFF ) {
      
    case 0x7:
      std::cout << "SET REG1 TIMER_DELAY\n";
      registers[opcode & 0x0F00] = timer_delay;
      break;

    case 0x0A:
      std::cout << "NOT DONE LOAD KEY VX\n";
      break;

    case 0x15:
      std::cout << "SET TIMER_DELAY REG1\n";
      timer_delay = registers[opcode & 0x0F00];
      break;

    case 0x18:
      std::cout << "SET TIMER_SOUND REG1\n";
      timer_sound = registers[opcode & 0x0F00];
      break;

    case 0x1E:
      std::cout << "ADD I I+REG1\n";
      index += registers[opcode & 0x0F00];
      break;

    case 0x29:
      std::cout << "NOT DONE LOAD F VX???\n";
      
      break;

    case 0x33:
      std::cout << "NOT DONE LOAD B VX???\n";
      break;

    case 0x55:
      std::cout << "LOAD I VX???\n";
      for ( auto i = 0; i <= (opcode & 0x0F00 >> 8); ++i ) {
        memory[index + i] = registers[i];
      }
      break;

    case 0x65:
      std::cout << "LOAD VX I ???\n";
      for ( auto i = 0; i <= (opcode & 0x0F00 >> 8); ++i ) {
        registers[i] = memory[index + i];
      }
      break;
    }
    break;
    
  default:
    std::cerr << "Could not read OPCODE: " << std::hex << opcode << ", "<< std::dec << program_counter << std::endl;
    break;
  }
  
  program_counter += 2; 
}


void Chip8::decrease_timers() {
  if ( timer_delay > 0 ) --timer_delay;
  if ( timer_sound > 0 ) --timer_sound;
}


int main(int argc, char *argv[]) {
  if ( argc != 2 ) {
    std::cout << "Usage: idc8in 'ROM_LOCATION'" << std::endl;
    return 1;
  }
  
  Chip8 chip8;

  if ( chip8.load_rom(argv[1]) ) {
    chip8.cycle();
  }
  
  return 0;
}
