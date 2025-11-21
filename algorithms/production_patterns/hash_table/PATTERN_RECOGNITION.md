# Hash Table Pattern Recognition

## When to Recognize Hash Table Opportunity

### Input Characteristics That Suggest Hash Table

1. **Key-Value Storage**
   - Need to store key-value pairs
   - Fast lookup by key required
   - Dynamic set of keys

2. **O(1) Lookup Requirement**
   - Need constant-time average lookup
   - Faster than O(log n) tree-based structures
   - Frequent lookups

3. **Unordered Data**
   - Keys don't need to be sorted
   - No range queries needed
   - Simple existence/retrieval

4. **Variable Key Set**
   - Keys added/removed dynamically
   - Unknown key set size at compile time
   - Dynamic resizing needed

## Variant Selection Guide

### Decision Tree

```
Need hash table?
├─ High concurrency / Many readers?
│  └─ YES → Linux Kernel Lock-Free (RCU)
├─ Need O(1) worst-case lookup?
│  └─ YES → Cuckoo Hashing
├─ Need non-blocking resize?
│  └─ YES → Redis Open Addressing (Incremental Rehashing)
├─ Memory efficiency important?
│  └─ YES → PostgreSQL Chaining
├─ Want better cache performance?
│  └─ YES → Robin Hood Hashing
└─ Standard use case
   └─ PostgreSQL Chaining or Redis Open Addressing
```

### Open Addressing vs Chaining

**Open Addressing** (Redis, Cuckoo, Robin Hood):
- **Pros**: Better cache locality, no pointer overhead, simpler memory management
- **Cons**: Lower load factors, clustering issues, complex deletion
- **Use When**: Cache performance critical, memory overhead matters, predictable access patterns

**Chaining** (PostgreSQL, Linux Kernel):
- **Pros**: Higher load factors, simpler deletion, handles collisions gracefully
- **Cons**: Pointer overhead, worse cache locality, memory fragmentation
- **Use When**: Variable key sizes, high load factors needed, simpler implementation preferred

### Specific Variant Selection

#### Redis Open Addressing (Incremental Rehashing)

**Use When**:
- Need non-blocking hash table operations
- Real-time systems where blocking is unacceptable
- High-performance caching systems
- Security-sensitive (uses SipHash)

**Key Features**:
- Two hash tables for incremental rehashing
- Power-of-2 sizes (bitwise modulo)
- Progressive rehashing (one bucket per operation)
- No blocking during resize

**Trade-offs**:
- More memory (two tables during rehashing)
- Slightly more complex implementation
- Better for write-heavy workloads

#### PostgreSQL Chaining

**Use When**:
- Need predictable worst-case performance
- Memory efficiency is important
- Variable-length keys
- Standard hash table use cases

**Key Features**:
- Separate chaining for collisions
- Dynamic resizing
- Simple implementation
- Good for general-purpose use

**Trade-offs**:
- Pointer overhead per entry
- Worse cache locality than open addressing
- Simpler but potentially slower

#### Linux Kernel Lock-Free (RCU)

**Use When**:
- High-concurrency read-heavy workloads
- Many readers, few writers
- Kernel-level code
- Need to avoid reader-writer lock overhead

**Key Features**:
- RCU (Read-Copy-Update) for lock-free reads
- Memory barriers for multi-core safety
- Intrusive data structures
- Lock-free iteration

**Trade-offs**:
- More complex implementation
- Requires RCU understanding
- Better for read-heavy workloads
- Grace period overhead for writes

#### Cuckoo Hashing

**Use When**:
- Need guaranteed O(1) worst-case lookup
- Can tolerate occasional rehashing
- Simple implementation preferred
- Read-heavy workloads

**Key Features**:
- O(1) worst-case lookup (only two locations to check)
- Two hash tables with two hash functions
- Kick-out strategy on collision
- Simple and elegant

**Trade-offs**:
- Lower load factor (typically 0.5)
- May need rehashing on insertion cycles
- Better for read-heavy workloads
- More memory (two tables)

#### Robin Hood Hashing

**Use When**:
- Need better cache performance than standard open addressing
- Want reduced variance in probe lengths
- High load factors acceptable
- Read-heavy workloads

**Key Features**:
- Reduced variance in probe lengths
- Better cache performance
- Backward shift deletion
- Higher load factors (0.8-0.9)

**Trade-offs**:
- More complex insertion logic
- Distance tracking overhead
- Better cache performance
- More uniform probe distribution

## Input Characteristics → Variant Mapping

| Input Characteristic | Recommended Variant | Reason |
|---------------------|---------------------|--------|
| High concurrency, many readers | Linux Kernel Lock-Free | RCU provides lock-free reads |
| Need O(1) worst-case lookup | Cuckoo Hashing | Only two locations to check |
| Non-blocking resize required | Redis Open Addressing | Incremental rehashing |
| Memory efficiency critical | PostgreSQL Chaining | No extra table overhead |
| Cache performance critical | Robin Hood Hashing | Better cache locality |
| Standard use case | PostgreSQL Chaining | Good general-purpose choice |
| Security-sensitive | Redis Open Addressing | Uses SipHash |
| High load factors needed | Robin Hood Hashing | Can handle 0.8-0.9 load factor |
| Write-heavy workload | Redis Open Addressing | Incremental rehashing helps |
| Read-heavy workload | Cuckoo Hashing | O(1) worst-case lookup |

## Performance Characteristics Comparison

| Variant | Average Insert | Worst Insert | Average Lookup | Worst Lookup | Load Factor |
|---------|---------------|-------------|----------------|--------------|-------------|
| Redis Open Addressing | O(1) | O(n) during rehash | O(1) | O(n) during rehash | 0.7-0.8 |
| PostgreSQL Chaining | O(1) | O(k) k=chain length | O(1) | O(k) k=chain length | 0.75 |
| Linux Kernel Lock-Free | O(1) | O(1) + grace period | O(1) | O(k) k=chain length | 0.75 |
| Cuckoo Hashing | O(1) expected | O(n) if rehash | O(1) | O(1) | 0.5 |
| Robin Hood Hashing | O(1) | O(log n) | O(1) | O(log n) | 0.8-0.9 |

## Real-World Examples

### Redis Open Addressing
- **Redis Database**: All key-value operations use incremental rehashing
- **High-Performance Caching**: Non-blocking cache operations
- **Real-Time Systems**: Where blocking is unacceptable

### PostgreSQL Chaining
- **PostgreSQL Hash Indexes**: Database hash indexes
- **PostgreSQL Hash Joins**: Join algorithm hash tables
- **General-Purpose Hash Tables**: Standard use cases

### Linux Kernel Lock-Free
- **Process Management**: Process hash tables
- **File Descriptor Tables**: FD hash tables
- **Network Subsystem**: Network connection hash tables

### Cuckoo Hashing
- **Network Routers**: Fast packet lookup tables
- **Compiler Symbol Tables**: Fast symbol resolution
- **Database Indexes**: O(1) lookup requirement

### Robin Hood Hashing
- **Game Engines**: Fast entity lookup
- **Compiler Symbol Tables**: Better cache performance
- **High-Performance Hash Tables**: Reduced variance

## Pattern Recognition Checklist

Before choosing a hash table variant, ask:

1. **What's the concurrency pattern?**
   - Many readers? → Linux Kernel Lock-Free
   - Many writers? → Redis Open Addressing
   - Low concurrency? → Any variant

2. **What's the lookup requirement?**
   - O(1) worst-case? → Cuckoo Hashing
   - O(1) average acceptable? → Any variant

3. **What's the resize requirement?**
   - Non-blocking? → Redis Open Addressing
   - Blocking acceptable? → Any variant

4. **What's the memory constraint?**
   - Tight memory? → PostgreSQL Chaining
   - Memory available? → Any variant

5. **What's the cache performance need?**
   - Critical? → Robin Hood Hashing or Redis Open Addressing
   - Not critical? → PostgreSQL Chaining

6. **What's the load factor expectation?**
   - High (>0.8)? → Robin Hood Hashing
   - Medium (0.7-0.8)? → Redis Open Addressing or PostgreSQL Chaining
   - Low (<0.5)? → Cuckoo Hashing

## Common Mistakes to Avoid

1. **Using wrong collision resolution**
   - Open addressing for high load factors
   - Chaining when cache performance critical

2. **Ignoring concurrency**
   - Using non-thread-safe variant in multi-threaded code
   - Not considering reader-writer patterns

3. **Wrong load factor**
   - Too high load factor for open addressing
   - Too low load factor wasting memory

4. **Not considering resize cost**
   - Blocking resize in real-time systems
   - Frequent resizes due to poor initial size

5. **Ignoring cache performance**
   - Using chaining when cache performance matters
   - Not considering probe sequence locality

## Decision Flow Examples

### Example 1: High-Performance Cache

**Problem**: Need fast cache with non-blocking operations

**Pattern Recognition**:
- Key-value storage → Hash table
- Non-blocking → Redis Open Addressing
- High performance → Open addressing for cache locality

**Variant**: Redis Open Addressing

### Example 2: Compiler Symbol Table

**Problem**: Fast symbol lookup with O(1) worst-case

**Pattern Recognition**:
- Key-value storage → Hash table
- O(1) worst-case → Cuckoo Hashing
- Read-heavy → Cuckoo Hashing benefits

**Variant**: Cuckoo Hashing

### Example 3: Kernel Process Table

**Problem**: Process lookup with many readers

**Pattern Recognition**:
- Key-value storage → Hash table
- Many readers → Linux Kernel Lock-Free
- Kernel code → RCU pattern

**Variant**: Linux Kernel Lock-Free

### Example 4: Database Hash Index

**Problem**: General-purpose hash index

**Pattern Recognition**:
- Key-value storage → Hash table
- Standard use case → PostgreSQL Chaining
- Memory efficient → Chaining preferred

**Variant**: PostgreSQL Chaining

## Universal Applications

Each variant has universal applications:

- **Redis Open Addressing**: Caching, real-time systems, high-performance storage
- **PostgreSQL Chaining**: Databases, general-purpose hash tables, memory-efficient systems
- **Linux Kernel Lock-Free**: Kernel code, high-concurrency systems, read-heavy workloads
- **Cuckoo Hashing**: Network routers, compilers, O(1) worst-case requirements
- **Robin Hood Hashing**: Game engines, high-performance systems, cache-critical applications

