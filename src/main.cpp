#include "chrono"
#include "ring_buffer.hpp"
#include "thread"
#include "types.hpp"
#include <chrono>
#include <iostream>

int main() {
  RingBuffer<TickData> rb(1024);

  std::thread producer([&]() {
    for (int i = 0; i < 5; ++i) {
      TickData tick;
      tick.exchange_timestamp = i;
      tick.open = 100.0 + i;

      if (rb.push(tick)) {
        std::cout << "[Producer] Pushed tick with price: " << tick.open
                  << std::endl;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  });

  std::thread consumer([&]() {
    int count = 0;
    while (count < 5) {
      TickData tick;
      if (rb.pop(tick)) {
        std::cout << "[Consumer] Popped tick! Price was " << tick.open
                  << std::endl;
        count++;
      } else {
      }
    }
  });

  producer.join();
  consumer.join();

  std::cout << "Pipeline test complete!!" << std::endl;
  return 0;
};