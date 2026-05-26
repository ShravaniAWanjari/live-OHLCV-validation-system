# Part 4: Real-Time Analytics & Visualization Dashboard

### What I did:
1. **Aggregated Pipeline Metrics:** Set up the R visualization suite to read `metrics.csv`, which tracks raw handoff latency (in nanoseconds), candle prices, and validation flags emitted by the C++ engine.
2. **Computed Latency Statistics:** Added real-time analysis to compute total processed ticks, anomalous ticks, mean latency, p95, p99, and max latency (tail jitter) in microseconds.
3. **Engineered the Performance Timeline:** Built a timeline visualization plotting processing latency per tick, with a red warning threshold line at 20 microseconds to easily spot jitter spikes.
4. **Mapped Anomalies in Real-Time:** Plotted the active close price stream and overlayed flagged anomalies (such as invalid high/low/open/close bounds or sequence errors) with distinct red diamond markers to pinpoint where rules broke.
5. **Analyzed Pipeline Jitter:** Built a distribution histogram mapping processing latency to understand the tail latency behavior under load.
6. **Self-Contained Dashboard Generation:** Configured the script to generate a single high-fidelity, consolidated dashboard image and auto-save it to `dashboard_plots.png`.

### Why:
HFT pipelines require more than raw speed—they require deep visibility. This R dashboard bridges the low-level C++ telemetry with actionable insights, allowing us to inspect price anomalies, track tail jitter, and visualize how our optimizations behave under load.

### Version 1 Baseline Results:
When running the initial partially optimized pipeline with 100 ticks, we achieved the following baseline results:

```text
==================================================
          PIPELINE METRICS SUMMARY
==================================================
Total Processed Ticks : 100
Total Anomalous Ticks : 0
Mean Latency          : 10.389 microseconds
95th Percentile (p95) : 18.640 microseconds
99th Percentile (p99) : 45.955 microseconds
Max Latency           : 61.300 microseconds
==================================================
```

![Pipeline Analytics & Anomalies Dashboard](dashboard_plots.png)

**Status:** Instrumentation is complete and visualized. Note that this dashboard represents the initial, partially optimized version of our pipeline. We are now ready to implement the deep C++ hardware and algorithmic optimizations in **Version 2** to push the pipeline to its peak limits.

