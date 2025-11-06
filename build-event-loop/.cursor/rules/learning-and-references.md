# Learning and References

## What to learn (topics)
* IO multiplexing: epoll, kqueue, edge vs. level triggered
* Reactor architecture and per core sharding
* Timers: hierarchical wheels, min heap schedulers
* Backpressure and bounded queues
* NUMA, affinity, and cache friendly structures
* Observability: metrics, tracing, logs

## DSA foundations
* Ring buffers and circular queues
* Heaps and priority queues
* Hash maps with stable iteration for registries
* Lock free SPSC and MPSC queues basics

## How to improve
* Replace min heap timers with wheel when counts are high
* Convert blocking paths to offloaded workers with backpressure
* Reduce syscalls via batching and coalescing
* Profile and remove cache misses and branch mispredicts

## Research papers and usage
* Timer wheels: apply for large timer counts; document jitter bounds
* Readiness vs. completion models: justify reactor choice and its impact
* Lock free queues: use only where contention justifies complexity; prove correctness

## Open source to study
* libuv, libevent, seastar

## Practice plan
* Week 1: single thread reactor with timers
* Week 2: per core reactors with reuseport and backpressure
* Week 3: observability and performance hardening
