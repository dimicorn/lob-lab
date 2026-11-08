# LOB-Lab — Project Overview

**A mixed-language market-data replay, limit-order-book reconstruction, and machine-learning research platform.**

**Languages:** Rust · C++20 · Python 3.12+  
**Target platform:** Linux x86-64  
**Focus:** Quantitative research engineering, high-performance systems, reproducible ML  
**Status:** Planned / educational portfolio project

## 1. Motivation

LOB-Lab is a hands-on systems and research project designed to bridge three disciplines:

- **Modern systems programming:** memory ownership, data structures, correctness, interoperability, and performance measurement.
- **Quantitative engineering:** market-data processing, order-book reconstruction, replay, simulation, and latency-aware infrastructure.
- **Machine learning research:** feature engineering, time-series prediction, uncertainty, distribution shift, and rigorous out-of-sample evaluation.

The project is intended to demonstrate skills useful for **Quantitative Research Engineer**, **Quantitative Developer**, **ML Systems Engineer**, and **C++/Rust Trading Infrastructure Engineer** roles. It is **not** intended as a production trading platform or a promise of profitable trading strategies.

The defining idea is to combine a genuinely useful **Rust ingestion layer** with a personally engineered **C++20 event-processing core**, then connect both to a **Python-based research workflow**.

## 2. Project at a glance

| Subsystem | Language | Responsibility |
|---|---|---|
| Market-data ingestion | **Rust** | Streaming parsing, validation, canonicalization, binary encoding/decoding |
| Event interoperability | **Rust + C ABI + C++** | Defined event representation, batch transfer, ownership and lifetime contracts |
| Order-book reconstruction | **C++20** | Deterministic historical event application, price-level and order-ID indexing |
| Replay and feature extraction | **C++20** | Time-ordered processing, spread, depth, imbalance, and related features |
| Model research | **Python** | Baselines, PyTorch/LightGBM experiments, chronological evaluation |
| Model inference | **C++20** | ONNX Runtime deployment, numerical-parity tests, latency measurements |
| Backtesting | **C++20 + Python** | Event-driven evaluation with explicitly stated fill and cost assumptions |
| Performance analysis | **Rust + C++20** | Throughput, p50/p95/p99 latency, allocations, cache effects, profiling |

### Conceptual data flow

```text
Historical order-book messages (CSV / feed-specific format)
                           |
                           v
               Rust market-data ingestion
          Parse -> validate -> canonical events
                           |
                           v
               Rust/C++ batch-oriented FFI
               (documented C ABI boundary)
                           |
                           v
               C++20 event replay engine
                           |
                           v
              C++20 reconstructed order book
                       /         \
                      v           v
               Feature engine   Correctness /
                    |           performance tests
                    v
             Python ML research
             -> model export (ONNX)
                    |
                    v
             C++ model inference
                    |
                    v
         Event-driven strategy evaluation
          -> metrics, reports, benchmarks
```

**Important distinction:** Historical *reconstruction* applies events recorded by a market-data source. It must not invent exchange matching behavior. Simulated order matching, if added, is a separate model with explicitly documented assumptions.

## 3. Functional scope

### Rust: market-data processing

Rust is a **mandatory, first-class subsystem**, not an optional showcase.

- Read source files efficiently, initially using streaming CSV input.
- Validate event fields and report malformed records with useful diagnostics.
- Normalize source-specific event types into a versioned internal event schema.
- Encode and decode a binary event format with explicit byte order and versioning.
- Provide a batched C-compatible interface for passing events to the C++ engine.
- Test parsing, codec round-trips, and cross-language integration.
- Benchmark ingestion and decoding independently of the order-book engine.

**Learning goals:** ownership and borrowing, lifetimes, slices, traits, error handling, safe/unsafe boundaries, FFI design, layout and alignment, and performance profiling. Start synchronously; async networking is not needed for historical replay.

### C++20: reconstruction and replay

- Represent prices using **integer ticks** and quantities using integer types; avoid floating-point price equality.
- Track order IDs, price levels, aggregate quantities, and priority information appropriate to the source feed.
- Support applicable add, cancel, delete, execute, and replace/update events.
- Process events deterministically and preserve chronological/sequence semantics.
- Extract quantitative features and expose reproducible feature outputs.
- Add clear behavior for invalid events, sequence gaps, and empty/locked/crossed books.

**Learning goals:** RAII, ownership and lifetime, container design, iterator validity, algorithmic complexity, cache behavior, allocators, compiler optimization, and measured performance.

### Python: machine-learning research

- Train simple and strong baselines before complex architectures.
- Start with a majority-class predictor, logistic regression, gradient boosting, and a small MLP.
- Predict short-horizon price direction or return using clearly specified event-time or clock-time horizons.
- Use **chronological, leakage-aware evaluation**, ideally separating trading days or sessions and handling overlapping labels.
- Measure predictive quality independently of strategy profitability.
- Export an appropriate model to ONNX and validate parity with C++ inference.

**Research themes:** time-series nonstationarity, domain/regime shift, feature usefulness, calibration, robustness, and the trade-off between predictive improvement and inference cost.

### Backtesting: research-grade simulation

Only after replay and inference are correct, introduce a basic event-driven strategy evaluation layer:

- Positions, inventory constraints, fees, transaction costs, and simulated latency.
- Explicit quote, order, queue-position, and fill assumptions.
- Net/gross P&L, turnover, inventory exposure, drawdown, and relevant risk measures.
- Separate results for prediction metrics, execution assumptions, and simulated financial outcomes.

**Limitation:** A displayed historical quote does not guarantee a fill, and historical replay cannot faithfully reproduce how a real market would have reacted to a hypothetical order. Do not overstate simulated performance.

## 4. Interoperability contract

The Rust/C++ boundary is a deliberate engineering focus.

- Use a **stable C ABI** for the initial integration, favoring **event batches** over one call per event.
- Define fixed-width fields and explicit event/tag values; never pass native Rust containers or C++ standard-library objects through the ABI.
- Document C-compatible struct layout, alignment, padding, integer widths, representation, and ownership.
- Document which side allocates and releases each buffer, and how long borrowed views remain valid.
- Prevent Rust unwinding and C++ exceptions from crossing the ABI boundary.
- Return explicit error/status codes and test invalid inputs.
- Verify ABI layouts, codec round-trips, and buffer lifetimes in integration tests.
- Benchmark different batch sizes to determine where FFI overhead becomes material.

Do not claim zero-copy processing unless the implementation actually avoids copying and establishes valid alignment, layout, and lifetime guarantees.

## 5. Engineering and testing principles

**Correctness before performance.** First build a deterministic, single-threaded implementation. Add concurrency only if measurements justify it and if correctness can be demonstrated.

Recommended development infrastructure:

- **CMake + CTest** for C++; **Cargo workspace** for Rust.
- **GoogleTest** and **Google Benchmark**; Rust's built-in test framework and a suitable benchmark framework.
- **clang-format, clang-tidy, rustfmt, Clippy**.
- Compiler warnings and reproducible Debug/Release build presets.
- **AddressSanitizer and UndefinedBehaviorSanitizer**; **ThreadSanitizer** when threaded code is introduced.
- Continuous integration that builds and tests both languages.
- Linux profiling with `perf` or equivalent tools where available.

Important tests include:

1. Known event sequences with expected best bid/ask, quantities, and FIFO ordering.
2. Partial/full execution, cancellation, deletion, replacement, and unknown IDs.
3. Duplicate records, malformed events, empty books, and sequence gaps.
4. Randomized/property-based comparisons against a deliberately simple reference model.
5. Deterministic replay from identical input.
6. Rust codec round-trips and Rust-to-C++ fixture parity.
7. Python/C++ model inference parity within stated numerical tolerances.
8. End-to-end feature timing and no-lookahead checks.

## 6. Performance methodology

Benchmark each stage separately:

- Rust CSV parsing and validation.
- Rust binary decoding.
- Rust/C++ event transfer at multiple batch sizes.
- C++ order-book updates.
- C++ feature extraction.
- C++ model inference.
- Complete replay pipeline.

Collect **throughput**, **latency distributions (p50/p95/p99 when meaningful)**, **memory use**, and **allocation counts** where available. Separate disk I/O, initialization, parsing, processing, and inference. Record CPU, compiler, build flags, dataset, input size, warm-up method, and repetitions. Treat optimizations as controlled experiments: form a hypothesis, measure a baseline, make one change, check correctness, and rerun the benchmark.

Potential subjects include cache locality, branch prediction, memory layout, allocations, SIMD, and batch sizing. Lock-free structures and custom allocators are not requirements unless profiling identifies a compelling use case.

## 7. Development milestones

| Milestone | Outcome | Definition of done |
|---|---|---|
| **M0 — Toolchain** | Cargo/CMake workspace and minimal Rust-to-C++ call | Reproducible build, CI, basic test pass |
| **M1 — Event schema** | Canonical events, binary format, ABI/ownership specification | Cross-language fixture and layout tests pass |
| **M2 — Rust ingestion** | Streaming parser, validation, codec | Known datasets parse; round-trips and parser tests pass |
| **M3 — C++ order book** | Single-threaded deterministic reconstruction and replay | Invariants, edge cases, and reference-model tests pass |
| **M4 — Quant features** | C++ features exported for Python | Correct timing, reference calculations, reproducibility |
| **M5 — ML + inference** | Baselines, temporal evaluation, exported inference | Python/C++ numerical parity and benchmark report |
| **M6 — Performance** | Controlled profiling and optimization experiments | Reproducible benchmark report and documented trade-offs |
| **M7 — Backtesting** | Cost-aware event-driven simulation | Fill assumptions and limitations documented and tested |

**Portfolio MVP:** M0–M3, including a convincing correctness suite and honest performance measurements.  
**Hybrid ML + C++ showcase:** M0–M6.  
**Advanced extension:** M7, plus carefully justified concurrency or deeper Rust optimization.

## 8. Proposed repository layout

```text
lob-lab/
├── README.md
├── overview.md
├── CMakeLists.txt
├── CMakePresets.json
├── Cargo.toml
├── include/
│   ├── lob/
│   └── ffi/
├── src/
│   ├── book/
│   ├── replay/
│   ├── features/
│   └── inference/
├── rust/
│   ├── market_data/
│   ├── event_codec/
│   └── ffi/
├── apps/
├── tests/
├── benchmarks/
├── python/
│   ├── data/
│   ├── models/
│   └── evaluation/
├── docs/
│   ├── architecture/
│   ├── experiments/
│   └── performance/
└── .github/workflows/
```

This is a **target structure**, not a requirement to create every empty directory on day one. Keep the initial repository minimal.

## 9. Data and reproducibility

Start with a clearly documented public or properly licensed historical order-book sample (for example, a LOBSTER sample). Understand the event semantics and license before implementing the parser. Limited sample data is suitable for **correctness and prototype experiments**, not robust profitability conclusions. For serious cross-day generalization experiments, obtain an appropriate multi-session dataset, subject to access, cost, and licensing.

Do not commit restricted datasets or large binary artifacts. Provide instructions to acquire data and reproduce derived results.

## 10. Human–AI collaboration rules

This project is meant to **develop the author's engineering skill**, not simply generate a polished repository.

**Human-owned implementation:**

- Rust parser/codec core and FFI design.
- C++ order-book data structures, event application, and ownership decisions.
- Performance-critical optimization and concurrency decisions.
- Benchmark interpretation and technical trade-off analysis.

**AI-assisted or delegated work:**

- Build tooling, dependency setup, CI, formatting and linting.
- Test scaffolding, fixture generation, routine Python utilities, and documentation.
- Design reviews, code reviews, edge-case discovery, profiling suggestions, and incremental hints.

For core work, the AI should **explain requirements, offer design alternatives, ask for the author's proposal, review it, and give hints**. It must not silently implement or rewrite the core logic. Full implementations require an explicit request.

## 11. Definition of success

The project succeeds when its author can:

- Explain every important Rust/C++ ownership and FFI decision.
- Defend the C++ containers and their operation complexities.
- Reconstruct order-book state deterministically from documented event semantics.
- Demonstrate tests that catch subtle correctness failures.
- Show reproducible, interpretable performance experiments.
- Explain latency vs throughput and the cost of allocations, cache misses, and FFI calls.
- Train and evaluate an ML model without temporal leakage.
- Explain the limitations of simulated fills and backtested P&L.
- Modify and optimize the core independently in a technical interview.

**The main artifact is the engineering knowledge demonstrated by the system—not an artificially impressive throughput number or a purportedly profitable strategy.**

## 12. First concrete step

Set up a minimal **Cargo workspace + CMake project** that can build a trivial Rust library and call it from a C++ executable via a documented C ABI. Test it in CI. Then, before writing the parser or order book, design the **canonical event schema and buffer-ownership contract** and review their invariants.
