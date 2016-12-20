#ifndef CPU_HPP
#define CPU_HPP

#include <array>

class Chip8 {
public:
  Chip8();
  ~Chip8();
  bool load_rom(const char *ROM);
  void cycle();
  void decrease_timers();
  void decode_opcode();

  bool draw;
  std::array<uint8_t, 16> keyboard;
  std::array<uint8_t, 2048> screen; 
  const char beep = '\a';
  uint8_t timer_delay, timer_sound;
  
private:
  std::array<uint8_t, 4096> memory;
  uint16_t opcode;
  std::array<uint8_t, 16> registers;
  uint16_t index, program_counter, stack_pointer;
  std::array<uint16_t, 16> stack;
};

#endif
