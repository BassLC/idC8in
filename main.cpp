#include <iostream>
#include <fstream>

int main() {
  std::ifstream ROM ("/home/bernardo/dev/C++/idC8in/ROMS/PONG", std::ios::binary); //(ROM_location);
  uint8_t i[4096];
 
  if ( ROM ) {
    std::cout << "Rom is cool!\n";
    for ( int l = 0; l < 200; ++l ) {
      ROM.read((char*)&i[l], 1);
    }
    ROM.close();

    uint16_t opcode = i[1]; //<< 8; //| i[2];
    
    std::cout << (uint16_t)opcode << "\n";

  }

  return 0;
  
}
