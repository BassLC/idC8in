#include "cpu.hpp"
#include <iostream>
#include "SDL2/SDL.h"


SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* texture = nullptr;

bool init() {
     if ( SDL_Init(SDL_INIT_EVERYTHING) != 0 ) {
          std::cout << "SDL Error: " << SDL_GetError() << std::endl;
          return false;
     }
     window = SDL_CreateWindow("Idiot('s) CHIP-8 Interpretor",
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               64, 32,
                               SDL_WINDOW_SHOWN);
     if ( !window ) {
          std::cout << "SDL Error (Window): " << SDL_GetError() << std::endl;
          return false;
     }
     renderer = SDL_CreateRenderer(window, -1,
                                   SDL_RENDERER_ACCELERATED);
  
     if ( !renderer ) {
          std::cout << "SDL_Error (Renderer): " << SDL_GetError() << std::endl;
          return false;
     }

     SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
     texture = SDL_CreateTexture(renderer,
                                 SDL_PIXELFORMAT_ABGR8888,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 64, 32);

     if ( !texture ) {
          std::cout << "SDL_Error (Texture): " << SDL_GetError() << std::endl;
          return false;
     }

     return true;
}

bool key_pressed(const SDL_Scancode keymap) {
     const uint8_t *keystates = SDL_GetKeyboardState(NULL);

     if ( keystates[keymap] ) { return true; }

     return false;
}

int main (int argc, char* argv[]) {
     std::array<SDL_Scancode, 16> keymap = {
          SDL_SCANCODE_1,
          SDL_SCANCODE_2,
          SDL_SCANCODE_3,
          SDL_SCANCODE_4,
          SDL_SCANCODE_Q,
          SDL_SCANCODE_W,
          SDL_SCANCODE_E,
          SDL_SCANCODE_R,
          SDL_SCANCODE_A,
          SDL_SCANCODE_S,
          SDL_SCANCODE_D,
          SDL_SCANCODE_F,
          SDL_SCANCODE_Z,
          SDL_SCANCODE_X,
          SDL_SCANCODE_C,
          SDL_SCANCODE_V,
     };
  
     if ( argc != 2 ) {
          std::cout << "Usage: idc8in 'ROM_LOCATION'" << std::endl;
          return 1;
     }
     Chip8 chip8;

     if ( chip8.load_rom(argv[1]) ) {
          if ( init() ) {
               bool exit = false;
               int last_moment = 0;
               int passed = SDL_GetTicks();
               SDL_Event e;
      
               while ( !exit ) {       

                    if ( passed - last_moment >= 4 ) { //It should be 16, I think, but it gets super slow... So 4 it is!
                         if ( chip8.timer_sound > 0 ) { std::cout << chip8.beep; }
        
                         if ( chip8.timer_delay == 0) {
                              chip8.decode_opcode();

                              while ( SDL_PollEvent(&e) ) {
                                   if ( e.type == SDL_QUIT || e.key.keysym.sym == SDLK_k ) { exit = true; }
                              }
            
                              for ( auto i = 0; i < 16; ++i ) {
                                   chip8.keyboard[i] = (key_pressed(keymap[i])) ? 1 : 0;
                              }
            

                              if ( chip8.draw ) {
                                   chip8.draw = false;
                                   uint32_t pixels[2048];
                                   for ( int i = 0; i < 2048; ++i ) { pixels[i] = (0x00FFFFFF * chip8.screen[i]) | 0xFF000000; }
              
                                   SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(uint32_t));
                                   SDL_RenderClear(renderer);
                                   SDL_RenderCopy(renderer, texture, NULL, NULL);
                                   SDL_RenderPresent(renderer);
                              }
                         }
        
                         chip8.decrease_timers();
                         last_moment = passed;
                    }
                    passed = SDL_GetTicks();
               }
          }
     }
  
     return 0;
}
