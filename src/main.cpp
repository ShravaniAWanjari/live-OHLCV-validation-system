
#include "ingestor.hpp"
#include "logger.hpp"
#include "ring_buffer.hpp"
#include "risk_manager.hpp"
#include "types.hpp"
#include "validator.hpp"
#include <chrono>
#include <fstream>
#include <immintrin.h>
#include <iostream>
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <thread>
#include <vector>

struct MetricRecord {
  uint64_t exchange_timestamp;
  uint64_t arrival_timestamp;
  uint64_t processed_timestamp;
  uint64_t latency_ns;
  double close;
  double volume;
  uint8_t validation_flags;
};

void run_edge_case_tests() {
  std::cout << "\n=============================================" << std::endl;
  std::cout << "RUNNING PIPELINE EDGE CASE TESTS..." << std::endl;
  std::cout << "=============================================" << std::endl;
  Validator test_validator;
  RiskManager test_risk_manager(3);

  TickData base_tick;
  base_tick.exchange_timestamp = 1000;
  base_tick.open = 100.0;
  base_tick.high = 105.0;
  base_tick.low = 95.0;
  base_tick.close = 102.0;
  base_tick.volume = 10.0;

  // Test 1
  std::cout << "\n[Test 1] Injecting clean tick..." << std::endl;
  uint8_t err1 = test_validator.validate(base_tick);
  test_risk_manager.handle_validation_result(err1, "Test 1 Context");
  std::cout << "System Active: " << std::boolalpha
            << test_risk_manager.is_active() << std::endl;

  // Test 2
  std::cout << "\n[Test 2] Injecting crossed OHLC tick (High: 90, Low: 95)..."
            << std::endl;
  TickData bad_ohlc = base_tick;
  bad_ohlc.exchange_timestamp = 1001;
  bad_ohlc.high = 90.0;
  bad_ohlc.low = 95.0;

  uint8_t err2 = test_validator.validate(bad_ohlc);
  test_risk_manager.handle_validation_result(err2, "Test 2 Context");
  std::cout << "System Active: " << std::boolalpha
            << test_risk_manager.is_active() << " (Expected: false)"
            << std::endl;

  test_validator.reset();
  test_risk_manager.reset();

  // Test 3
  std::cout << "\n[Test 3] Injecting chronologically stale tick..."
            << std::endl;
  TickData tick_t1 = base_tick;
  tick_t1.exchange_timestamp = 2000;
  test_validator.validate(tick_t1);

  TickData tick_t2 = base_tick;
  tick_t2.exchange_timestamp = 1999;
  uint8_t err3 = test_validator.validate(tick_t2);
  test_risk_manager.handle_validation_result(err3, "Test 3 Context");
  std::cout << "System Active: " << std::boolalpha
            << test_risk_manager.is_active() << " (Expected: false)"
            << std::endl;

  test_validator.reset();
  test_risk_manager.reset();

  // Test 4
  std::cout << "\n[Test 4] Injecting 50% price spike tick..." << std::endl;
  TickData tick_p1 = base_tick;
  tick_p1.exchange_timestamp = 3000;
  tick_p1.close = 100.0;
  test_validator.validate(tick_p1);

  TickData tick_p2 = base_tick;
  tick_p2.exchange_timestamp = 3001;
  tick_p2.close = 150.0;
  uint8_t err4 = test_validator.validate(tick_p2);
  test_risk_manager.handle_validation_result(err4, "Test 4 context");
  std::cout << "System Active: " << std::boolalpha
            << test_risk_manager.is_active() << " (Expected: false)"
            << std::endl;

  test_validator.reset();
  test_risk_manager.reset();

  // Test 5: Negative Volume Accumulation (Non-critical)
  std::cout << "\n[Test 5] Injecting negative volume ticks (non-critical limit "
               "= 3)..."
            << std::endl;
  TickData tick_v = base_tick;
  tick_v.volume = -1.0;

  for (int i = 1; i <= 4; ++i) {
    tick_v.exchange_timestamp = 4000 + i;
    std::cout << "Injecting anomaly #" << i << std::endl;
    uint8_t err_v = test_validator.validate(tick_v);
    test_risk_manager.handle_validation_result(err_v, "Test 5 Context");
    std::cout << "System Active: " << std::boolalpha
              << test_risk_manager.is_active() << std::endl;
  }
  std::cout << "=============================================\n" << std::endl;
}

int main() {

  run_edge_case_tests();

  ix::initNetSystem();

  RingBuffer<TickData> rb(1024);
  Ingestor ingestor;
  Validator validator;
  RiskManager risk_manager(3);

  ix::WebSocket webSocket;
  std::string url = "wss://stream.binance.com:9443/ws/btcusdt@kline_1s";
  webSocket.setUrl(url);

  ix::SocketTLSOptions tlsOptions;
  tlsOptions.caFile = "NONE";
  tlsOptions.disable_hostname_validation = true;
  webSocket.setTLSOptions(tlsOptions);

  const int target_ticks = 100;
  AsyncLogger<MetricRecord> logger("metrics.csv");

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
  logger.start();
  webSocket.start();

  std::thread consumer([&]() {
    int count = 0;
    std::vector<uint64_t> latencies;
    latencies.reserve(target_ticks);

    while (count < target_ticks) {
      TickData tick;
      if (rb.pop(tick)) {
        if (!risk_manager.is_active()) {
          std::cerr << "[Consumer] Downstream pipeline is Halted. Dropping "
                       "incoming feeds."
                    << std::endl;
          break;
        }
        auto now = std::chrono::high_resolution_clock::now();
        uint64_t process_time =
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                now.time_since_epoch())
                .count();

        uint8_t validation_err = validator.validate(tick);

        logger.log({tick.exchange_timestamp, tick.arrival_timestamp,
                    process_time, process_time - tick.arrival_timestamp,
                    tick.close, tick.volume, validation_err});

        risk_manager.handle_validation_result(validation_err,
                                              "Live Binance Stream");
        if (!risk_manager.is_active()) {
          std::cerr
              << "[Consumer] Risk Manager initiated Halt. Aborting Consumer."
              << std::endl;
          break;
        }
        if (validation_err != VALID) {
          std::cout << "[Consumer] Anomalous tick dropped (Flags: "
                    << (int)validation_err << ")" << std::endl;
          continue;
        }
        latencies.push_back(process_time - tick.arrival_timestamp);
        count++;
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

  logger.stop();

  std::cout << "\nStopping WebSocket..." << std::endl;
  webSocket.stop();
  ix::uninitNetSystem();

  std::cout << "Live pipeline test completed successfully!" << std::endl;
  std::cout << "Final Stats - Total: " << validator.get_total_ticks()
            << " | Failures: " << validator.get_failure_count() << std::endl;
  return 0;
}