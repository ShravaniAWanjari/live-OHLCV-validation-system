#pragma once
#include "ring_buffer.hpp"
#include <string>
#include <fstream>
#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>

template <typename T>
class AsyncLogger {
public:
  AsyncLogger(const std::string& filename, size_t queue_size = 4096)
      : queue_(queue_size), filename_(filename), running_(false) {}

  ~AsyncLogger() {
    stop();
  }

  // Starts the background logging thread and opens the CSV file
  void start() {
    if (running_) return;

    file_.open(filename_);
    if (!file_.is_open()) {
      std::cerr << "[Logger] Failed to open " << filename_ << std::endl;
      return;
    }

    // Write the CSV header
    file_ << "exchange_timestamp,arrival_timestamp,processed_timestamp,latency_ns,close,volume,validation_flags\n";
    running_ = true;
    worker_ = std::thread(&AsyncLogger::log_loop, this);
  }

  // Gracefully stops the thread and flushes remaining items
  void stop() {
    if (!running_) return;

    running_ = false;
    if (worker_.joinable()) {
      worker_.join();
    }

    flush_queue();
    if (file_.is_open()) {
      file_.close();
    }
  }

  // Pushes a log record into the lock-free queue (called by hot path)
  bool log(const T& record) {
    return queue_.push(record);
  }

private:
  // Loop run by the background thread
  void log_loop() {
    while (running_) {
      T record;
      if (queue_.pop(record)) {
        write_record(record);
      } else {
        // Sleep for a short duration when there are no logs to save CPU cycles
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
      }
    }
  }

  // Dumps any remaining logs in the queue during shutdown
  void flush_queue() {
    T record;
    while (queue_.pop(record)) {
      write_record(record);
    }
  }

  void write_record(const T& record) {
    file_ << record.exchange_timestamp << ","
          << record.arrival_timestamp << ","
          << record.processed_timestamp << ","
          << record.latency_ns << ","
          << record.close << ","
          << record.volume << ","
          << static_cast<int>(record.validation_flags) << "\n";
  }

  RingBuffer<T> queue_;
  std::string filename_;
  std::ofstream file_;
  std::thread worker_;
  std::atomic<bool> running_;
};
