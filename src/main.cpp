#include "ingestor.hpp"
#include "ring_buffer.hpp"
#include "types.hpp"
#include <chrono>
#include <immintrin.h>
#include <iostream>
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <thread>
#include <vector>

int main() {

  ix::initNetSystem();

  RingBuffer<TickData> rb(1024);
  Ingestor ingestor;

  ix::WebSocket webSocket;
  std::string url = "wss://stream.binance.com:9443/ws/btcusdt@kline_1s";
  webSocket.setUrl(url);

  ix::SocketTLSOptions tlsOptions;
  tlsOptions.caFile = "NONE";
  tlsOptions.disable_hostname_validation = true;
  webSocket.setTLSOptions(tlsOptions);

  const int target_ticks = 10;

  webSocket.setOnMessageCallback([&](const ix::WebSocketMessagePtr &msg) {
    if (msg->type == ix::WebSocketMessageType::Message) {
      TickData tick;

      auto now = std::chrono::high_resolution_clock::now();
      tick.arrival_timestamp =
          std::chrono::duration_cast<std::chrono::nanoseconds>(
              now.time_since_epoch())
              .count();

      if (ingestor.parse_kline(msg->str, tick)) {
        if (!rb.push(tick)) {
          std::cerr << "[Warning] Ring buffer full, dropped tick." << std::endl;
        }
      }
    } else if (msg->type == ix::WebSocketMessageType::Error) {
      std::cerr << "WebSocket Error: " << msg->errorInfo.reason << std::endl;
    }
  });

  std::cout << "Connecting to Binance Live WebSocket (BTCUSDT 1s stream)..."
            << std::endl;
  webSocket.start();

  std::thread consumer([&]() {
    int count = 0;
    std::vector<uint64_t> latencies;
    latencies.reserve(target_ticks);

    while (count < target_ticks) {
      TickData tick;
      if (rb.pop(tick)) {
        auto now = std::chrono::high_resolution_clock::now();
        uint64_t process_time =
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                now.time_since_epoch())
                .count();

        latencies.push_back(process_time - tick.arrival_timestamp);
        count++;

        std::cout << "[Consumer] Processed live tick " << count << "/"
                  << target_ticks << " | Close: " << tick.close
                  << " | Vol: " << tick.volume << std::endl;
      } else {
        _mm_pause();
      }
    }

    std::cout << "\n--- Latency Results for Live Binance Feed ---" << std::endl;
    for (size_t i = 0; i < latencies.size(); ++i) {
      std::cout << "Live Tick " << i + 1 << " Latency: " << latencies[i]
                << " ns" << std::endl;
    }
  });

  consumer.join();

  std::cout << "\nStopping WebSocket..." << std::endl;
  webSocket.stop();
  ix::uninitNetSystem();

  std::cout << "Live pipeline test completed successfully!" << std::endl;
  return 0;
}