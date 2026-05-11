# Live OHLCV Validation System (HFT-Lite)

A high-performance, real-time data validation engine built with C++20. This system is designed to ingest live market data, validate it with microsecond precision, and flag anomalies before they hit a trading strategy.

## 1. Objective
Build a production-grade system that:
* Ingests live OHLCV data via WebSockets.
* Validates data quality in real-time (OHLC consistency, volume checks, gaps).
* Tracks ingestion-to-validation latency.
* Demonstrates low-latency engineering (Cache alignment, Lock-free SPSC queues, SIMD parsing).

## 2. Architecture (HFT-Lite)
The system uses a decoupled, multi-threaded architecture to ensure zero-allocation steady states and minimal jitter.

```
[Exchange WebSocket]
        ↓
[Producer Thread: Ingestor]  <-- Boost.Beast + simdjson
        ↓
[Lock-Free SPSC Ring Buffer] <-- Handover bridge (No Mutexes!)
        ↓
[Consumer Thread: Validator] <-- Core Math & Anomaly Logic
        ↓
[Logging / Analytics]
```

## 3. Tech Stack
* **Language:** C++20 (for performance and modern concurrency features).
* **Build System:** CMake + Ninja.
* **Networking:** `Boost.Asio` / `Boost.Beast` (Asynchronous WebSockets).
* **JSON Parsing:** `simdjson` (High-throughput SIMD parsing).
* **Concurrency:** Atomic-based SPSC (Single-Producer Single-Consumer) lock-free ring buffer.

## 4. Development Roadmap

### Phase 1: Infrastructure (Completed)
- [x] Project skeleton and CMake setup.
- [x] Cache-aligned `TickData` struct (64-byte alignment).
- [x] Lock-free SPSC Ring Buffer with Acquire/Release memory ordering.
- [x] Basic multi-threaded test pipeline.

### Phase 2: Live Ingestion
- [ ] Integrate `simdjson` for fast parsing.
- [ ] Set up `Boost.Beast` WebSocket client.
- [ ] Connect to Binance live stream.

### Phase 3: Validation Engine
- [ ] Implement Level 1 checks (High >= Open/Close/Low, etc.).
- [ ] Implement gap detection and timestamp sequence validation.

### Phase 4: Latency & Analytics
- [ ] Measure end-to-end latency (arrival vs. processed time).
- [ ] Build a simple Python dashboard to visualize flags and latency spikes.

## 5. How to Build and Run

### Prerequisites
* CMake (3.15+)
* Ninja build system
* MinGW-w64 (GCC) or MSVC

### Commands
```powershell
# 1. Configure
cmake -G Ninja -S . -B build

# 2. Build
cmake --build build

# 3. Run Test Pipeline
.\build\validator.exe
```
