#pragma once
#include <cstdint>

struct TickData {
  uint64_t timestamp;

  double open;
  double high;
  double low;
  double close;
  double volume;
};