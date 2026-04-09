# C++ HFT-Lite Data Validation Pipeline (Optimal Architecture)

This plan outlines how we will build the core ingestion and validation engine entirely in C++ using HFT (High-Frequency Trading) principles. We are skipping the Python prototype to focus directly on low-latency, memory-optimized architecture.

## User Review Required

Please review this revised C++ architecture. Since you requested the "most optimal shit", this plan incorporates industry-standard HFT-lite techniques. If this looks good, we will start with **Phase 1: Build Setup & Data Structures**. I will provide the C++ code block by block so you can write and understand it.

## Architecture & Dependencies

To achieve minimal latency and zero-allocation steady states, we will use the following tools:
- **Build System**: CMake (C++20 recommended)
- **Networking**: `Boost.Asio` and `Boost.Beast` (standard for asynchronous WebSockets)
- **JSON Parsing**: `simdjson` (an insanely fast, SIMD-accelerated JSON parser built for high throughput)
- **Concurrency**: A Single-Producer-Single-Consumer (SPSC) lock-free ring buffer (e.g., `moodycamel::ReaderWriterQueue` or a custom ring buffer) to pass raw ticks from the network thread to the validation thread without mutex locks.

## Proposed Steps

### Step 1: Project Skeleton & Core Structs (`src/types.hpp`, `CMakeLists.txt`)
- Set up CMake to pull GoogleTest, Boost, and simdjson.
- Define a cache-line aligned (64-byte or 32-byte) `TickData` struct to ensure it fits perfectly into CPU cache lines. It will store timestamps (exchange vs. arrival), OHLC float/double values, and volume.

### Step 2: The Lock-Free Ring Buffer (`src/ring_buffer.hpp`)
- We will implement or drop in a lock-free SPSC queue. This ensures that the thread reading from the WebSocket never gets slowed down by the validation logic.

### Step 3: Network Ingestion Layer (`src/ingestor.hpp/cpp`)
- Implement the WebSocket connection to the Binance trade or Kline stream using `Boost.Beast`.
- When a payload arrives, immediately timestamp its arrival.
- Parse the JSON using `simdjson`, construct the `TickData` struct, and emplace it onto the lock-free queue. This prevents any memory allocations during the hot loop.

### Step 4: Core Validation Engine (`src/validator.hpp/cpp`)
- Run this engine on a separated dedicated thread (the "Consumer").
- Spin-wait on the ring buffer to grab new ticks as fast as possible.
- Perform `Level 1` validation rules:
  - `High >= max(Open, Close, Low)`
  - `Low <= min(Open, Close, High)`
  - `Volume >= 0`
  - Gap and missing timestamp detection.

### Step 5: Output / Logging (`src/main.cpp`)
- Wire the producer and consumer threads together.
- Output clean ticks to a fast logger/CSV, and keep track of caught anomalies to demonstrate the value of this validation layer.

## Open Questions

- **OS/Compiler**: Since you're on Windows, do you plan to use MSVC, MinGW (GCC), or WSL (Windows Subsystem for Linux)? I recommend WSL or MSVC for optimal C++20 support.
- **Queue Implementation**: Do you want me to write a custom lock-free ring buffer from scratch (great for learning/interviews), or use a battle-tested library like `moodycamel::ReaderWriterQueue`? 

## Verification Plan

### Automated Tests
- We can write basic GTest cases later, but first, we want the system running.

### Manual Verification
- We will run the compiled executable, connect it to the live stream, and measure the end-to-end latency (arrival time - parse time - validation exact time). HFT style!
