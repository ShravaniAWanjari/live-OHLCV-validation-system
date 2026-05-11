#pragma once
#include <atomic>
#include <optional>
#include <vector>

template <typename T> class RingBuffer {
public:
  explicit RingBuffer(size_t size)
      : size_(size), buffer_(size), head_(0), tail_(0) {}

  bool push(const T &value) {
    size_t current_tail = tail_.load(std::memory_order_relaxed);

    size_t next_tail = (current_tail + 1) % size_;

    if (next_tail == head_.load(std::memory_order_acquire)) {
      return false;
    }

    buffer_[current_tail] = value;

    tail_.store(next_tail, std::memory_order_release);
    return true;
  }

  bool pop(T &value) {
    size_t current_head = head_.load(std::memory_order_relaxed);
    if (current_head == tail_.load(std::memory_order_acquire)) {
      return false;
    }

    value = buffer_[current_head];

    head_.store((current_head + 1) % size_, std::memory_order_release);
    return true;
  }

private:
  size_t size_;
  std::vector<T> buffer_;

  std::atomic<size_t> head_;
  std::atomic<size_t> tail_;
};