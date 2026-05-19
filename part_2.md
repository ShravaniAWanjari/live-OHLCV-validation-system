# Part 2: JSON Parsing & Microsecond Optimizations

### What I did:
1. **Integrated simdjson:** Set up the parser to read Binance WebSocket Kline streams in one fast SIMD sweep.
2. **Zero-Allocation Parser:** Rewrote the double parser to use C++20 `std::from_chars` and string views. This completely stops the compiler from requesting memory from the Operating System during the hot path.
3. **Killed False Sharing:** Added `alignas(64)` to the head and tail indices of the ring buffer. This keeps them on separate CPU cache lines so the threads don't block each other.
4. **Cleaned the Hot Path:** Removed slow printing (`std::cout`) from the hot path, saving the latencies in a pre-allocated vector to print after the loops finish.
5. **CPU micro-napping:** Put `_mm_pause()` in the empty-loop check so the CPU doesn't choke when waiting for ticks.

### The Results:
* **Cold start:** 49.5 microseconds.
* **Warm runs:** ~7.4 microseconds average handoff time.
