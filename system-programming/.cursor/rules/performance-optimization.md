# Performance Optimization

## Scope
Applies to profiling, benchmarking, performance analysis, optimization techniques, and performance measurement for system programming code.

## Profiling Tools

### CPU Profiling
* Use perf for CPU profiling
* Use gprof for function level profiling
* Use Intel VTune for detailed analysis
* Profile hot paths and bottlenecks

### Memory Profiling
* Use Valgrind for memory profiling
* Use AddressSanitizer (ASAN) for memory errors
* Use Massif for heap profiling
* Profile memory allocation patterns

### System Call Profiling
* Use strace for system call tracing
* Count system calls per operation
* Measure system call latency
* Identify system call bottlenecks

## Benchmarking

### Benchmarking Methodology
* Use consistent measurement methodology
* Warm up before timing
* Measure multiple times and take median
* Report p50, p95, p99 latencies
* Document test environment

### Benchmark Tools
* Use Google Benchmark for C++ code
* Use custom timing for C code
* Measure throughput and latency
* Profile under realistic workloads

### Performance Metrics
* Throughput: Operations per second
* Latency: Response time percentiles
* Resource usage: CPU, memory, I/O
* Scalability: Performance with load

## Optimization Techniques

### I/O Optimization
* Use memory mapped I/O for large files
* Use asynchronous I/O for high throughput
* Optimize buffer sizes
* Minimize system call overhead
* Use zero copy techniques

### Memory Optimization
* Minimize allocations in hot paths
* Use memory pools for frequent allocations
* Optimize data structure layout
* Reduce cache misses
* Use NUMA aware allocation

### CPU Optimization
* Optimize hot paths
* Reduce branch mispredictions
* Use SIMD when appropriate
* Minimize function call overhead
* Profile and optimize critical loops

## Performance Analysis

### Identifying Bottlenecks
* Profile to find hot paths
* Measure system call overhead
* Analyze cache performance
* Identify lock contention
* Profile memory allocation

### Optimization Strategies
* Measure before optimizing
* Optimize hot paths first
* Verify optimizations improve performance
* Document performance improvements
* Consider trade offs (complexity, portability)

## Code Examples

### Benchmarking Pattern
```cpp
// Thread-safety: Thread-safe (read-only)
// Ownership: None (measurement only)
// Invariants: Function to benchmark valid
// Failure modes: None
void benchmark_operation() {
    auto start = std::chrono::high_resolution_clock::now();
    // Operation to benchmark
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        end - start);
    std::cout << "Operation took: " << duration.count() << " microseconds\n";
}
```

## Testing Requirements
* Benchmark critical operations
* Profile under realistic workloads
* Measure performance regressions
* Verify optimization improvements
* Document performance characteristics

## Related Topics
* File Operations: I/O performance optimization
* Thread Management: Thread performance
* Network Programming: Network performance optimization
* Synchronization: Lock performance
* Memory Management: Memory optimization
* Platform-Specific: Platform-specific optimizations

