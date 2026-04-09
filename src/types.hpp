#pragma once
#include <cstdint>

namespace hft{
    struct alignas(64) TickData{
        double timestamp;
        double open;
        double high;
        double low;
        double close;
        double volume;

        uint32_t symbol_id
    // In HFT, we almost never use std::string in hot paths because it heap-allocates.
    // Instead, we map string symbols (e.g., "BTCUSDT") to integer IDs.
        uint32_t flags;
    };
};