# Hash Table Extraction Notes

## Summary

Extracted 5 hash table variants from multiple sources:
- **Redis** (GitHub): Open addressing with incremental rehashing
- **PostgreSQL** (GitHub): Separate chaining with dynamic resizing
- **Linux Kernel** (Local): Lock-free with RCU support
- **Cuckoo Hashing** (Research): Two hash tables with O(1) worst-case lookup
- **Robin Hood Hashing** (Research): Open addressing with distance tracking

## Extracted Variants

### 1. Redis Open Addressing

**Source**: https://github.com/redis/redis/blob/unstable/src/dict.c
**Repository**: redis/redis
**File**: `src/dict.c`
**Variant File**: `production_patterns/hash_table/variants/redis_open_addressing.cpp`

**Key Features**:
- Two hash tables (`ht[0]` and `ht[1]`) for incremental rehashing
- Power-of-2 table sizes (bitwise modulo: `hash & mask`)
- SipHash for security (resistant to hash flooding)
- Progressive rehashing: moves one bucket per operation
- Non-blocking operations during resize

**Key Insights**:
- Incremental rehashing eliminates blocking during resize
- Power-of-2 sizes enable fast bitwise modulo (no expensive division)
- Two-table approach allows seamless transition during resize
- Operations check both tables when rehashing is in progress

**Performance Characteristics**:
- Insert: O(1) average, O(n) worst case (during rehashing)
- Search: O(1) average, O(n) worst case (during rehashing)
- Delete: O(1) average, O(n) worst case (during rehashing)
- Load Factor: 0.7-0.8 typically

**Use Cases**:
- High-performance caching systems
- Real-time systems where blocking is unacceptable
- Security-sensitive applications (SipHash)
- Write-heavy workloads

### 2. PostgreSQL Chaining

**Source**: https://github.com/postgres/postgres/blob/master/src/backend/utils/hash/dynahash.c
**Repository**: postgres/postgres
**File**: `src/backend/utils/hash/dynahash.c`
**Variant File**: `production_patterns/hash_table/variants/postgresql_chaining.cpp`

**Key Features**:
- Separate chaining for collision resolution
- Dynamic hash table growth (doubles size when needed)
- Memory-efficient design (only allocates chains as needed)
- Flexible hash function support
- Simple deletion (just remove from chain)

**Key Insights**:
- Chaining handles collisions gracefully without clustering
- Dynamic resizing maintains good performance
- Simpler implementation than open addressing
- Better for variable-length keys

**Performance Characteristics**:
- Insert: O(1) average, O(k) worst case where k is chain length
- Search: O(1) average, O(k) worst case where k is chain length
- Delete: O(1) average, O(k) worst case where k is chain length
- Load Factor: 0.75 typically

**Use Cases**:
- General-purpose hash tables
- Database hash indexes
- Hash joins in databases
- Memory-efficient systems

### 3. Linux Kernel Lock-Free

**Source**: `/Users/picon/Learning/c-or-c-plus-plus/linux/include/linux/hashtable.h`
**File**: `linux/include/linux/hashtable.h`
**Variant File**: `production_patterns/hash_table/variants/linux_kernel_lockfree.cpp`

**Key Features**:
- RCU (Read-Copy-Update) for lock-free reads
- Separate chaining with hlist (head-only list)
- Power-of-2 table sizes
- Memory barriers for multi-core safety
- Intrusive data structures (no extra allocations)
- Lock-free iteration with RCU

**Key Insights**:
- RCU enables lock-free reads (no reader locks needed)
- Memory barriers ensure visibility across cores
- Intrusive structures reduce memory overhead
- Grace period mechanism for safe memory reclamation
- Excellent for read-heavy workloads

**Performance Characteristics**:
- Insert: O(1) average (with RCU grace period)
- Search: O(1) average, O(k) worst case where k is chain length
- Delete: O(1) average (with RCU grace period)
- Load Factor: 0.75 typically

**Use Cases**:
- Kernel-level code
- High-concurrency read-heavy workloads
- Systems with many readers, few writers
- Real-time systems requiring predictable latency

### 4. Cuckoo Hashing

**Source**: "Cuckoo Hashing" by Rasmus Pagh and Flemming Friche Rodler
**Paper**: ESA 2001 (European Symposium on Algorithms)
**Variant File**: `production_patterns/hash_table/variants/cuckoo_hashing.cpp`

**Key Features**:
- Two hash tables with two independent hash functions
- O(1) worst-case lookup (only two locations to check)
- Kick-out strategy: evicts existing element on collision
- Simple and elegant algorithm
- Good cache performance

**Key Insights**:
- Two hash functions provide two possible locations
- Kick-out strategy maintains O(1) lookup guarantee
- May need rehashing if insertion cycles occur
- Lower load factor (0.5) but better worst-case performance

**Performance Characteristics**:
- Insert: O(1) expected, O(n) worst case (requires rehashing)
- Search: O(1) worst case (only two locations to check)
- Delete: O(1) worst case
- Load Factor: 0.5 typically

**Use Cases**:
- Network routers (fast packet lookup)
- Compiler symbol tables
- Database indexes requiring O(1) worst-case lookup
- Read-heavy workloads

### 5. Robin Hood Hashing

**Source**: "Robin Hood Hashing" by Pedro Celis
**Paper**: University of Waterloo Technical Report CS-86-14 (1986)
**Variant File**: `production_patterns/hash_table/variants/robin_hood_hashing.cpp`

**Key Features**:
- Open addressing with distance tracking
- Reduced variance in probe lengths
- Better cache performance than standard open addressing
- Backward shift deletion (maintains probe order)
- "Steal from the rich, give to the poor" swapping strategy

**Key Insights**:
- Distance tracking enables uniform probe distribution
- Swapping entries balances probe lengths
- Backward shift deletion maintains order
- Higher load factors (0.8-0.9) than standard open addressing
- Better cache locality than chaining

**Performance Characteristics**:
- Insert: O(1) average, O(log n) worst case
- Search: O(1) average, O(log n) worst case
- Delete: O(1) average with backward shift
- Load Factor: 0.8-0.9 typically

**Use Cases**:
- Game engines (fast entity lookup)
- Compiler symbol tables
- High-performance hash tables
- Cache-critical applications

## Comparison of Variants

### Collision Resolution

| Variant | Method | Pros | Cons |
|---------|--------|------|------|
| Redis | Open Addressing | Cache locality, no pointers | Lower load factor, clustering |
| PostgreSQL | Chaining | Higher load factor, simple | Pointer overhead, worse cache |
| Linux Kernel | Chaining + RCU | Lock-free reads | RCU complexity |
| Cuckoo | Two Tables | O(1) worst-case | Lower load factor, rehashing |
| Robin Hood | Open Addressing | Reduced variance | Distance tracking overhead |

### Concurrency Support

| Variant | Concurrency | Method |
|---------|-------------|--------|
| Redis | Single-threaded (or external locking) | No built-in concurrency |
| PostgreSQL | External locking | No built-in concurrency |
| Linux Kernel | Lock-free reads | RCU mechanism |
| Cuckoo | External locking | No built-in concurrency |
| Robin Hood | External locking | No built-in concurrency |

### Memory Efficiency

| Variant | Memory Overhead | Notes |
|---------|----------------|-------|
| Redis | Medium (two tables during rehash) | Extra table during rehashing |
| PostgreSQL | Low (only chain pointers) | Minimal overhead |
| Linux Kernel | Low (intrusive structures) | No extra allocations |
| Cuckoo | Medium (two tables always) | Always maintains two tables |
| Robin Hood | Low (distance tracking) | Small distance field overhead |

### Performance Summary

| Variant | Best For | Worst For |
|---------|----------|-----------|
| Redis | Write-heavy, non-blocking resize | Memory-constrained |
| PostgreSQL | General-purpose, memory-efficient | Cache-critical |
| Linux Kernel | Read-heavy, high concurrency | Write-heavy |
| Cuckoo | O(1) worst-case requirement | High load factors |
| Robin Hood | Cache performance, high load | Memory-constrained |

## Key Insights from Sources

### Redis Insights
1. **Incremental Rehashing**: Non-blocking resize is critical for real-time systems
2. **Power-of-2 Optimization**: Bitwise modulo is much faster than division
3. **Security**: SipHash prevents hash flooding attacks
4. **Progressive Migration**: Moving one bucket per operation spreads cost

### PostgreSQL Insights
1. **Simplicity**: Chaining is easier to implement correctly
2. **Flexibility**: Handles variable-length keys naturally
3. **Memory Efficiency**: Only allocates what's needed
4. **Predictability**: Chain length provides predictable worst-case

### Linux Kernel Insights
1. **RCU Pattern**: Lock-free reads are crucial for kernel performance
2. **Memory Barriers**: Critical for multi-core correctness
3. **Intrusive Structures**: Reduce memory overhead significantly
4. **Grace Periods**: Enable safe memory reclamation

### Cuckoo Hashing Insights
1. **Two Hash Functions**: Provide two chances for placement
2. **Kick-Out Strategy**: Maintains O(1) worst-case lookup
3. **Load Factor Trade-off**: Lower load factor for better worst-case
4. **Simplicity**: Elegant algorithm despite power

### Robin Hood Hashing Insights
1. **Distance Tracking**: Enables uniform probe distribution
2. **Swapping Strategy**: Balances probe lengths dynamically
3. **Backward Shift**: Maintains probe order on deletion
4. **Cache Performance**: Better than chaining, comparable to open addressing

## Performance Characteristics Summary

| Metric | Redis | PostgreSQL | Linux Kernel | Cuckoo | Robin Hood |
|--------|-------|------------|--------------|--------|------------|
| Avg Insert | O(1) | O(1) | O(1) | O(1) | O(1) |
| Worst Insert | O(n) | O(k) | O(1)+grace | O(n) | O(log n) |
| Avg Lookup | O(1) | O(1) | O(1) | O(1) | O(1) |
| Worst Lookup | O(n) | O(k) | O(k) | O(1) | O(log n) |
| Load Factor | 0.7-0.8 | 0.75 | 0.75 | 0.5 | 0.8-0.9 |
| Cache Locality | High | Medium | Medium | High | High |
| Concurrency | None | None | RCU | None | None |

## Use Case Recommendations

### Choose Redis Open Addressing When:
- Need non-blocking hash table operations
- Real-time systems where blocking is unacceptable
- High-performance caching systems
- Security-sensitive applications

### Choose PostgreSQL Chaining When:
- General-purpose hash table needs
- Memory efficiency is important
- Variable-length keys
- Standard use cases

### Choose Linux Kernel Lock-Free When:
- High-concurrency read-heavy workloads
- Kernel-level code
- Many readers, few writers
- Need to avoid reader-writer lock overhead

### Choose Cuckoo Hashing When:
- Need guaranteed O(1) worst-case lookup
- Can tolerate occasional rehashing
- Read-heavy workloads
- Simple implementation preferred

### Choose Robin Hood Hashing When:
- Need better cache performance than standard open addressing
- Want reduced variance in probe lengths
- High load factors acceptable
- Cache-critical applications

## Next Steps

1. Extract more hash table variants (e.g., Hopscotch Hashing, Linear Probing variants)
2. Benchmark variants for specific use cases
3. Document thread-safe wrappers for variants
4. Create performance comparison benchmarks

