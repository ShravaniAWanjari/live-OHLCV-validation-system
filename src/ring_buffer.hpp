#pragma once
#include <atomic>
#include <cstddef>
#include <optional>
#include <vector>

template <typename T> class RingBuffer {
public:
  explicit RingBuffer(size_t size)
      : capacity_(next_power_of_two(size)), mask_(capacity_ - 1),
        buffer_(capacity_), head_(0), cached_head_(0), tail_(0),
        cached_tail_(0) {}

  bool push(const T &value) {
    size_t current_tail = tail_.load(std::memory_order_relaxed);

    size_t next_tail = (current_tail + 1) & mask_;

    if (next_tail == cached_head_) {
      cached_head_ = head_.load(std::memory_order_acquire);
      if (next_tail == cached_head_) {
        return false;
      }
    }

    buffer_[current_tail] = value;

    tail_.store(next_tail, std::memory_order_release);
    return true;
  }

  bool pop(T &value) {
    size_t current_head = head_.load(std::memory_order_relaxed);
    if (current_head == cached_tail_) {
      cached_tail_ = tail_.load(std::memory_order_acquire);
      if (current_head == cached_tail_) {
        return false;
      }
    }

    value = buffer_[current_head];

    head_.store((current_head + 1) & mask_, std::memory_order_release);
    return true;
  }

private:
  size_t capacity_;
  size_t mask_;
  std::vector<T> buffer_;

  alignas(64) std::atomic<size_t> head_;
  alignas(64) size_t cached_head_;

  alignas(64) std::atomic<size_t> tail_;
  alignas(64) size_t cached_tail_;

  static size_t next_power_of_two(size_t n) {
    size_t count = 1;
    while (count < n)
      count <<= 1;
    return count;
  }
};