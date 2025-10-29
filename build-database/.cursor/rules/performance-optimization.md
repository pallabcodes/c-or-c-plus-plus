# Performance Optimization Standards

## Scope
Applies to all performance optimization code including SIMD, JIT compilation, vectorization, and NUMA awareness. Extends repository root rules.

## SIMD Operations

### SIMD Usage
* Vectorized arithmetic operations
* Parallel data processing
* AVX, AVX2, AVX 512 support
* Automatic vectorization where possible
* Manual SIMD intrinsics for critical paths

### Vectorized Operations
* Column wise operations
* Batch tuple processing
* Aggregation operations
* Predicate evaluation
* Reference: "Efficiently Compiling Efficient Query Plans for Modern Hardware" (Neumann, 2011)

## JIT Compilation

### Query Compilation
* Compile hot query paths
* LLVM backend integration
* Eliminate interpreter overhead
* Optimize tight loops
* Reference: "JIT-compiling SQL queries in MonetDB" (Neumann & Kemper, 2015)

### Code Generation
* Generate specialized code paths
* Constant folding
* Dead code elimination
* Register allocation optimization

## Vectorized Execution

### Batch Processing
* Process tuples in batches
* Reduce function call overhead
* Better cache utilization
* SIMD friendly data layout
* Reference: "MonetDB/X100: Hyper-Pipelining Query Execution" (Boncz et al., 2005)

### Column Oriented Execution
* Process columns in batch
* Cache efficient access patterns
* Parallel column operations
* Better compression

## NUMA Awareness

### NUMA Architecture
* Understand NUMA topology
* Local vs remote memory access
* Socket affinity
* Memory allocation policies

### NUMA Optimizations
* Allocate memory on local NUMA node
* Bind threads to NUMA nodes
* Minimize remote memory access
* NUMA aware data structures

## Cache Optimization

### Cache Conscious Design
* Cache line size awareness (64 bytes)
* Reduce false sharing
* Align data structures
* Prefetch strategies

### Memory Access Patterns
* Sequential access patterns
* Blocked algorithms
* Cache friendly data layouts
* Minimize cache misses

## Parallel Execution

### Inter Query Parallelism
* Process multiple queries concurrently
* Thread pool management
* Load balancing
* Resource isolation

### Intra Query Parallelism
* Parallel scans
* Parallel joins
* Parallel aggregations
* Pipeline parallelism
* Partition parallelism

## Profiling and Measurement

### Performance Profiling
* CPU profiling (perf, gprof)
* Memory profiling (valgrind)
* I/O profiling
* Cache profiling (perf cache)

### Benchmarking
* TPC benchmarks (TPC H, TPC C)
* YCSB workloads
* Custom workload generation
* Performance regression testing

## Implementation Requirements
* Profile before optimizing
* Measure impact of optimizations
* Maintain correctness
* Document performance characteristics
* Benchmark against industry standards

## Optimization Strategies
* Identify hot paths
* Optimize critical sections first
* Use appropriate data structures
* Minimize allocations
* Reduce system calls
* Batch operations

