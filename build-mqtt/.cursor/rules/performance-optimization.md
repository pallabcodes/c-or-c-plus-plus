# Performance Optimization

## Goals
* Sustain high connection counts and throughput with bounded latency (p50/p95/p99)

## Techniques
* Per-core reactors with SO_REUSEPORT accept sharding
* Batch event processing and coalesced acknowledgments
* Scatter/gather I O (writev/sendmsg); zero-copy encode where possible
* Buffer pools and slab allocators; avoid per-packet heap churn
* Cache hot-path computations; avoid branch mispredictions

## Backpressure
* Bounded queues per client; watermarks; slow-consumer policy

## Timers
* Wheel timers for keepalive and retries; batch expirations

## Profiling
* Periodic profiles on parse→route→send; record flamegraphs and regressions

## Targets
* Document SLOs and measured results in module README
