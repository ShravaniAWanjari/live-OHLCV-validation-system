#include "types.hpp"
#include <iostream>

int main() {
  TickData tick;

  std::cout << "Size of TickData " << sizeof(tick) << " bytes" << std::endl;

  return 0;
}