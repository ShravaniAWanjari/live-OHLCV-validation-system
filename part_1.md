# Part 1: SPSC lock free ring buffer 

### What I did:

1. **Set up the Build:** Got CMake and Ninja running so I can compile C++20 code properly.
2. **Optimized the Data:** Made a `TickData` struct and aligned it to 64 bytes (`alignas(64)`). This makes sure every tick fits perfectly in a CPU cache line so the hardware doesn't have to work extra hard.
3. **Built the "Shock Absorber":** Created a Lock-Free Ring Buffer (SPSC). This is the bridge between the network and the math.
4. **No Mutexes:** I avoided standard locks (mutexes) because they make the CPU sleep. Instead, I used `std::atomic` with Acquire/Release logic. It's way faster and keeps the threads moving at all times.
5. **Tested the Flow:** Wired up two threads in `main.cpp`. 
    *   **Producer:** Acting like the exchange, shoving data into the buffer.
    *   **Consumer:** Acting like the brain, pulling data out and processing it.

### Why this is cool:
The system is now "decoupled." The network thread can run at full speed without waiting for the logic thread to finish its math. This is how real HFT systems handle millions of messages without crashing.

**Status:** Infrastructure works. Next we plug in real data.
