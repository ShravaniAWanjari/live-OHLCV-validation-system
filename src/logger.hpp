#pragma once
#include "ring_buffer.hpp"
#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

template <typename T> class AsyncLogger {
public:
  AsyncLogger(const std::string &filename, size_t queue_size = 4096)
      : queue_(queue_size), filename_(filename), running_(false) {}

  ~AsyncLogger() { stop(); }

  void start() {
    if (running_)
      return;

    file_.open(filename_);
    if (!file_.is_open()) {
      std::cerr << "[Logger] Failed to open " << filename_ << std::endl;
      return;
    }

    file_ << "exchange_timestamp,arrival_timestamp,processed_timestamp,latency_"
             "ns,close,volume,validation_flags\n";
    running_ = true;
    worker_ = std::thread(&AsyncLogger::log_loop, this);
  }

  void stop() {
    if (!running_)
      return;

    running_ = false;
    if (worker_.joinable()) {
      worker_.join();
    }

    flush_queue();
    if (file_.is_open()) {
      file_.close();
    }
  }

  bool log(const T &record) { return queue_.push(record); }

private:
  void log_loop() {
#ifdef _WIN32
    SetThreadAffinityMask(GetCurrentThread(), 1ULL << 6);
#endif
    while (running_) {
      T record;
      if (queue_.pop(record)) {
        write_record(record);
      } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
      }
    }
  }

  void flush_queue() {
    T record;
    while (queue_.pop(record)) {
      write_record(record);
    }
  }

  void write_record(const T &record) {
    file_ << record.exchange_timestamp << "," << record.arrival_timestamp << ","
          << record.processed_timestamp << "," << record.latency_ns << ","
          << record.close << "," << record.volume << ","
          << static_cast<int>(record.validation_flags) << "\n";
  }

  RingBuffer<T> queue_;
  std::string filename_;
  std::ofstream file_;
  std::thread worker_;
  std::atomic<bool> running_;
};
