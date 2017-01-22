#include <iostream>
#include <fstream>
#include <random>
#include "cpu.hpp"


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
        screen.fill(0);
        keyboard.fill(0);

        const std::array<const uint8_t, 80> fontset = { 
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

        for ( std::size_t i = 0; i < 80; ++i ) { memory[i] = fontset[i]; } //Loading fontset
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
                        ROM.close();
                        return true;
      
                } else {
                        std::cerr << "ROM too big!\n";
                }
        }
        ROM.close();
        std::cerr << "Can't Read File.\n";
        return false;
}


void Chip8::decode_opcode() {
  
        //Fetch opcode
        opcode = memory[program_counter] << 8 | memory[program_counter + 1];

        //Defining constants
        const uint16_t X = (opcode & 0x0F00) >> 8;
        const uint16_t Y = (opcode & 0x00F0) >> 4;
        const uint16_t NNN = (opcode & 0x0FFF);
        const uint16_t KK = (opcode & 0x00FF);
        auto increment_pc = [this](){ program_counter += 2; };

        //Defining opcodes

        //0x0000
        const uint16_t CLR = 0x00E0;
        const uint16_t RET = 0x00EE;
  
        const uint16_t JMP = 0x1000;
        const uint16_t CALL = 0x2000;
        const uint16_t SE_REGX_KK = 0x3000;
        const uint16_t SNE_REGX_KK = 0x4000;
        const uint16_t SE_REGX_REGY = 0x5000;
        const uint16_t LOAD_REGX_KK = 0x6000;
        const uint16_t ADD_REGX_KK = 0x7000;
  
        //0x8000
        const uint16_t LOAD_REGX_REGY = 0x0000;
        const uint16_t OR_REGX_REGY = 0x0001;
        const uint16_t AND_REGX_REGY = 0x0002;
        const uint16_t XOR_REGX_REGY = 0x0003;
        const uint16_t ADD_REGX_REGY = 0x0004;
        const uint16_t SUB_REGX_REGY = 0x0005;
        const uint16_t SHR_REGX = 0x0006;
        const uint16_t SUBN_REGX_REGY = 0x0007;
        const uint16_t SHL_REGX = 0x000E;
  
        const uint16_t SNE_REGX_REGY = 0x9000;
        const uint16_t LOAD_INDEX_NNN = 0xA000;
        const uint16_t JMP_REG0_NNN = 0xB000;
        const uint16_t RND_REGX_KK = 0xC000;
        const uint16_t DRAW = 0xD000;

        //0xE000
        const uint16_t SKP_REGX = 0x009E;
        const uint16_t SKNP_REGX = 0x00A1;

        //0xF000
        const uint16_t LOAD_REGX_TD = 0x0007;
        const uint16_t LOAD_REGX_KEY = 0x000A;
        const uint16_t LOAD_TD_REGX = 0x0015;
        const uint16_t LOAD_TS_REGX = 0x0018;
        const uint16_t ADD_I_REGX = 0x001E;
        const uint16_t LOAD_F_VX = 0x0029;
        const uint16_t LOAD_BCD_MEM = 0x0033;
        const uint16_t LOAD_REGS_MEM = 0x0055;
        const uint16_t LOAD_MEM_REGS = 0x0065;

        //Random number generator
        const auto rand_numb = [](){
                const auto seed = std::random_device()();
                std::mt19937 generator(seed);
                std::uniform_int_distribution<int> ui(0, 255);
                return (uint8_t)ui(generator);
        };
  
  
        switch ( opcode & 0xF000 ) {

        case 0x0000:
                switch ( opcode & 0x00FF ) {
                        
                case CLR:
                        screen.fill(0);
                        draw = true;
                        increment_pc();
                        break;
    
                case RET:
                        --stack_pointer;
                        program_counter = stack[stack_pointer];
                        increment_pc();
                        break;
    
                }
                break;

        case JMP:
                program_counter = NNN;
                break;

        case CALL:
                stack[stack_pointer] = program_counter;
                ++stack_pointer;
                program_counter = NNN;
                break;
    
        case SE_REGX_KK:
                if ( registers[X] == KK ) { increment_pc(); }
                increment_pc();
                break;

        case SNE_REGX_KK:
                if ( registers[X] != KK ) { increment_pc();}
                increment_pc();
                break;

        case SE_REGX_REGY:
                if ( registers[X] == registers[Y] ) { increment_pc(); }
                increment_pc();
                break;

        case LOAD_REGX_KK:
                registers[X] = KK;
                increment_pc();
                break;

        case ADD_REGX_KK:
                registers[X] += KK;
                increment_pc();
                break;

        case 0x8000:
                switch ( opcode & 0x000F ) {
      
                case LOAD_REGX_REGY:
                        registers[X] = registers[Y];
                        increment_pc();
                        break;

                case OR_REGX_REGY:
                        registers[X] |= registers[Y];
                        increment_pc();
                        break;
    
                case AND_REGX_REGY:
                        registers[X] &= registers[Y];
                        increment_pc();
                        break;
      
                case XOR_REGX_REGY:
                        registers[X] ^= registers[Y];
                        increment_pc();
                        break;

                case ADD_REGX_REGY:
                        registers[0xF] = ( ((uint16_t)(registers[X] + registers[Y])) > 255 ) ? 1 : 0; // Carry
                        registers[X] += registers[Y];
                        increment_pc();
                        break;

                case SUB_REGX_REGY:
                        registers[0xF] = (registers[Y] > registers[X]) ? 1 : 0; //Carry
                        registers[X] -= registers[Y];
                        increment_pc();
                        break;

                case SHR_REGX:
                        registers[0xF] = registers[X] & 0x0001;
                        registers[X] >>= 1;
                        increment_pc();
                        break;

                case SUBN_REGX_REGY:
                        registers[0xF] = (registers[X] > registers[Y]) ? 0 : 1;
                        registers[X] = registers[Y] - registers[X];
                        increment_pc();
                        break;

                case SHL_REGX:
                        registers[0xF] = registers[X] >> 7;
                        registers[X] <<= 1;
                        increment_pc();
                        break;
        
                }
                break;

        case SNE_REGX_REGY:
                if ( registers[X] != registers[Y] ) { increment_pc(); }
                increment_pc();
                break;

        case LOAD_INDEX_NNN:
                index = NNN;
                increment_pc();
                break;

        case JMP_REG0_NNN:
                program_counter = NNN + registers[0];
                break;

        case RND_REGX_KK:
                registers[X] = KK & rand_numb();
                increment_pc();
                break;
    
        case DRAW:
        {
                unsigned short x = registers[X];
                unsigned short y = registers[Y];
                unsigned short height = opcode & 0x000F;

                registers[0xF] = 0;
                for ( int yline = 0; yline < height; yline++ )
                {
                        unsigned short pixel = memory[index + yline];
                        for ( int xline = 0; xline < 8; xline++ ) {
                                
                                if ( (pixel & (0x80 >> xline)) != 0 ) {
                                        
                                        if ( screen[(x+xline + ((y+yline) * 64))] == 1 ) {
                                                registers[0xF] = 1;
                                        }
                                        screen[x+xline + ((y+yline) * 64)] ^= 1;
                                }
                        }
                }

                draw = true;
                increment_pc();
        }
        break;

        case 0xE000:
                switch ( opcode & 0x00FF ) {

                case SKP_REGX:
                        if ( keyboard[registers[X]] != 0 ) { increment_pc(); }
                        increment_pc();
                        break;

                case SKNP_REGX:
                        if ( keyboard[registers[X]] == 0 ) { increment_pc(); }
                        increment_pc();
                        break;
                }
                break;

        case 0xF000:
                switch ( opcode & 0x00FF ) {

                case LOAD_REGX_TD:
                        registers[X] = timer_delay;
                        increment_pc();
                        break;

                case LOAD_REGX_KEY:
                {
                        bool pressed = false;
        
                        for ( int i = 0; i < 16; ++i ) {
                                if ( keyboard[i] != 0 ) {
                                        registers[X] = i;
                                        pressed = true;
                                }
                        }

                        if ( !pressed ) {return;}

                        increment_pc();
                }
                break;
      
                case LOAD_TD_REGX:
                        timer_delay = registers[X];
                        increment_pc();
                        break;

                case LOAD_TS_REGX:
                        timer_sound = registers[X];
                        increment_pc();
                        break;

                case ADD_I_REGX:
                        registers[0xF] = ( (uint16_t)(index + registers[X]) > 255 ) ? 1 : 0;
                        index += registers[X];
                        increment_pc();
                        break;

                case LOAD_F_VX:
                        index = registers[X] * 5; //Because 1 font element = 5 numbers;
                        increment_pc();
                        break;

                case LOAD_BCD_MEM:
                        memory[index] = registers[X] / 100;
                        memory[index + 1] = (registers[X] / 10) % 10;
                        memory[index + 2] = (registers[X] % 100) % 10;
                        increment_pc();
                        break;
    
                case LOAD_REGS_MEM:
                        for ( int i = 0; i <= X; ++i ) { memory[index + i] = registers[i]; }
                        index += X+1;
                        increment_pc();
                        break;

                case LOAD_MEM_REGS:
                        for ( int i = 0; i <= X; ++i ) { registers[i] = memory[index + i]; }
                        index += X+1;
                        increment_pc();
                        break;
                }
                break;
        
        default:
                std::cerr << "Could not read OPCODE: " << std::hex << opcode << ", "<< std::dec << program_counter << std::endl;
                increment_pc();
                break;
        }

}


void Chip8::decrease_timers() {
        if ( timer_delay > 0 ) --timer_delay;
        if ( timer_sound > 0 ) --timer_sound;
}
