#include "ingestor.hpp"
#include "ring_buffer.hpp"
#include "types.hpp"
#include <chrono>
#include <immintrin.h>
#include <iostream>
#include <thread>
#include <vector>

int main() {
  RingBuffer<TickData> rb(1024);
  Ingestor ingestor;

  std::string raw_binance_json = R"({
    "e": "kline",
    "E": 1626300000000,
    "s": "BTCUSDT",
    "k": {
      "t": 1626300000000,
      "T": 1626300059999,
      "s": "BTCUSDT",
      "i": "1m",
      "f": 100,
      "L": 200,
      "o": "34250.10",
      "c": "34280.20",
      "h": "34300.00",
      "l": "34200.50",
      "v": "15.424",
      "n": 10,
      "x": false,
      "q": "528500.23",
      "V": "7.21",
      "Q": "247000.12",
      "B": "0"
    }
  })";

  std::thread producer([&]() {
    for (int i = 0; i < 5; ++i) {
      TickData tick;

      auto now = std::chrono::high_resolution_clock::now();
      tick.arrival_timestamp =
          std::chrono::duration_cast<std::chrono::nanoseconds>(
              now.time_since_epoch())
              .count();

      if (ingestor.parse_kline(raw_binance_json, tick)) {
        if (rb.push(tick)) {
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  });

  std::thread consumer([&]() {
    int count = 0;

    std::vector<uint64_t> latencies;
    latencies.reserve(5);

    while (count < 5) {
      TickData tick;
      if (rb.pop(tick)) {
        auto now = std::chrono::high_resolution_clock::now();
        uint64_t process_time =
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                now.time_since_epoch())
                .count();

        latencies.push_back(process_time - tick.arrival_timestamp);
        count++;
      } else {
        _mm_pause();
      }
    }

    for (size_t i = 0; i < latencies.size(); ++i) {
      std::cout << "Tick " << i + 1 << " Latency: " << latencies[i] << " ns"
                << std::endl;
    }
  });

  producer.join();
  consumer.join();

  std::cout << "Pipeline test completed!!" << std::endl;
  return 0;
}