# Live OHLCV Validation Framework (HFT-lite) — Plan.md

---

# 1. Objective

Build a real-time system that:

* Ingests live OHLCV data
* Validates data quality in real-time
* Detects anomalies, latency issues, and inconsistencies
* Outputs clean + flagged data for downstream trading/research

Goal: Demonstrate production-grade thinking for quant dev + quant research roles

---

# 2. Tech Stack Decision

## Final Recommendation (Best Mix)

* C++ → Low-latency data ingestion + core validation engine
* Python → Orchestration, analysis, backtesting, dashboards
* (Optional) R → NOT needed (Python is sufficient)

## Why this works

* C++ shows systems + performance skills (huge signal)
* Python keeps development fast + flexible
* Avoid R → adds complexity without much benefit here

---

# 3. High-Level Architecture (HFT-lite)

```
[Exchange WebSocket]
        ↓
[C++ Data Ingestion Layer]
        ↓
[Stream Buffer / Queue]
        ↓
[C++ Validation Engine]
        ↓
[Anomaly Flag Stream]
        ↓
[Storage Layer (Parquet / DB)]
        ↓
[Python Analytics + Dashboard]
```

---

# 4. System Components

## 4.1 Data Ingestion (C++)

Responsibilities:

* Connect to exchange websocket (Binance, Zerodha, etc.)
* Parse tick / candle data
* Timestamp at arrival
* Push into buffer

Skills demonstrated:

* Networking
* Async handling
* Memory efficiency

---

## 4.2 Stream Buffer

Options:

* Simple: in-memory queue
* Advanced: Kafka / ZeroMQ

Recommendation:

* Start simple (queue)
* Upgrade later if needed

---

## 4.3 Validation Engine (Core)

### Level 1: Basic Checks

* OHLC consistency:

  * High >= max(Open, Close, Low)
  * Low <= min(Open, Close, High)

* Volume checks:

  * Volume >= 0

* Timestamp checks:

  * No duplicates
  * No missing intervals

---

### Level 2: Statistical Validation

* Z-score anomaly detection (price + volume)
* Rolling volatility bounds
* Gap detection

---

### Level 3: Advanced (10/10 feature)

* Cross-exchange validation
* Regime-aware thresholds (use clustering/Wasserstein)
* Microstructure-aware filtering

---

## 4.4 Latency Monitoring

Track:

* Exchange timestamp vs arrival timestamp

Metrics:

* Latency distribution
* Spike detection

---

## 4.5 Storage Layer

Options:

* Parquet files (best for analysis)
* SQLite/PostgreSQL (optional)

Store:

* Raw data
* Cleaned data
* Anomaly flags

---

## 4.6 Python Layer

Responsibilities:

### Analytics

* Data quality metrics
* Missing data stats
* Anomaly frequency

### Visualization

* Latency histograms
* Price before vs after cleaning
* Anomaly timelines

### Strategy Testing (VERY IMPORTANT)

Run simple strategies:

* Momentum
* Mean reversion

Compare:

* Raw data vs cleaned data

Metrics:

* Sharpe ratio
* Drawdown
* Signal noise reduction

---

# 5. Key Features That Make It 10/10

## MUST HAVE

* Real-time pipeline
* Modular validation system
* Latency tracking
* Clean vs raw comparison

## DIFFERENTIATORS

* Cross-exchange validation
* Regime-aware thresholds
* Strategy impact analysis

---

# 6. Development Roadmap

## Phase 1 (Week 1–2)

* Python-only prototype
* Simulated/live OHLCV ingestion
* Basic validation rules

## Phase 2 (Week 3–4)

* Move ingestion to C++
* Implement real-time pipeline

## Phase 3 (Week 5–6)

* Add statistical validation
* Add anomaly logging

## Phase 4 (Week 7–8)

* Add latency tracking
* Build dashboard (Python)

## Phase 5 (Week 9–10)

* Strategy backtesting
* Compare raw vs cleaned data

## Phase 6 (Bonus)

* Cross-exchange validation
* Regime detection integration

---

# 7. Resume Bullet (Final Output)

Built a low-latency OHLCV data validation framework using C++ and Python, capable of real-time anomaly detection, latency monitoring, and cross-exchange consistency checks. Demonstrated impact of data quality on trading strategies by improving Sharpe ratio and reducing false signals through cleaned data pipelines.

---

# 8. Interview Talking Points

* Why bad data is dangerous in trading
* Trade-off between latency vs validation strictness
* Design decisions (C++ vs Python split)
* How validation impacts alpha

---

# 9. Future Extensions

* Plug into live trading system
* Add ML-based anomaly detection
* Integrate order book data

---

# FINAL NOTE

Focus on:

* Clean architecture
* Measurable impact
* Real-time behavior

That is what makes this project stand out.
