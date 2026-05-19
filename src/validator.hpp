#pragma once
#include "types.hpp"
#include <cstdint>

enum ValidationFlags : uint8_t {
  VALID = 0,
  HIGH_LESS_THAN_LOW = 1 << 0,
  HIGH_LESS_THAN_OPEN = 1 << 1,
  HIGH_LESS_THAN_CLOSE = 1 << 2,
  LOW_GREATER_THAN_OPEN = 1 << 3,
  LOW_GREATER_THAN_CLOSE = 1 << 4,
  NEGATIVE_VOLUME = 1 << 5,
  OUT_OF_ORDER_TIMESTAMP = 1 << 6
};

class Validator {
public:
  Validator() : last_timestamp_(0), total_ticks_(0), failure_count_(0) {}

  uint8_t validate(const TickData &tick) {
    uint8_t result = VALID;
    total_ticks_++;

    if (tick.high < tick.low)
      result |= HIGH_LESS_THAN_LOW;
    if (tick.high < tick.open)
      result |= HIGH_LESS_THAN_OPEN;
    if (tick.high < tick.close)
      result |= HIGH_LESS_THAN_CLOSE;
    if (tick.low > tick.open)
      result |= LOW_GREATER_THAN_OPEN;
    if (tick.low > tick.close)
      result |= LOW_GREATER_THAN_CLOSE;

    if (tick.volume < 0.0)
      result |= NEGATIVE_VOLUME;

    if (last_timestamp_ != 0 && tick.exchange_timestamp <= last_timestamp_) {
      result |= OUT_OF_ORDER_TIMESTAMP;
    }

    if (result != VALID) {
      failure_count_++;
    } else {
      last_timestamp_ = tick.exchange_timestamp;
    }

    return result;
  }

  uint64_t get_total_ticks() const { return total_ticks_; }
  uint64_t get_failure_count() const { return failure_count_; }

private:
  uint64_t last_timestamp_;
  uint64_t total_ticks_;
  uint64_t failure_count_;
};
