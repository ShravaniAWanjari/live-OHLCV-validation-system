#pragma once
#include <cstdint>

struct alignas(64) TickData {
  uint64_t exchange_timestamp;
  uint64_t arrival_timestamp;

  double open;
  double high;
  double low;
  double close;
  double volume;
};
