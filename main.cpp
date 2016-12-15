#include <iostream>
#include <fstream>


int main() {
  std::ifstream ROM ("/home/bernardo/dev/C++/idC8in/ROMS/PONG"); //(ROM_location);

  if ( ROM ) {
    std::cout << "Rom is cool!\n";
    ROM.close();
    return true;
  }

  std::cerr << "Can't Read File.\n";
  return false;

}
