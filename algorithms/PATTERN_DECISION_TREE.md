# Pattern Recognition Decision Tree

## Master Decision Tree for Algorithm Selection

This document provides a comprehensive decision tree to help recognize which algorithm pattern to use based on problem characteristics.

## Entry Point: What Are You Trying to Do?

```
┌─────────────────────────────────────────────────────────┐
│  What problem are you solving?                         │
└─────────────────────────────────────────────────────────┘
                        │
        ┌───────────────┼───────────────┐
        │               │               │
    SEARCH          PROCESS         TRANSFORM
    (Find)         (Compute)        (Modify)
```

---

## SEARCH Problems

### Step 1: Is Data Sorted?

```
Is data sorted?
├─ YES → Go to SORTED SEARCH
└─ NO → Can it be sorted?
    ├─ YES → Sort first, then SORTED SEARCH
    └─ NO → Go to UNSORTED SEARCH
```

### SORTED SEARCH

```
Array size?
├─ ≤ 8 elements → LINEAR SEARCH (V8 optimization)
├─ > 8 elements
│   ├─ Searching by hash/computed key? → HASH-BASED BINARY SEARCH (V8)
│   ├─ Need insertion point for stable sort? → HYBRID BINARY+LINEAR (ICU)
│   ├─ Array size > INT_MAX/2? → OVERFLOW-SAFE BINARY SEARCH (V8 CodeGen)
│   └─ Standard case → STANDARD BINARY SEARCH
└─ Monotonic function? → BINARY SEARCH ON ANSWER SPACE
```

### UNSORTED SEARCH

```
Search type?
├─ Simple existence → LINEAR SEARCH or HASH TABLE
├─ Need frequency/count → HASH TABLE
└─ Need range queries → Consider sorting first or use TREE
```

---

## PROCESS Problems

### Step 1: What Are You Processing?

```
Processing what?
├─ Subarrays/Substrings → SLIDING WINDOW
├─ Pairs/Triplets → TWO POINTERS
├─ Multiple sequences → K-WAY MERGE
├─ Key-Value pairs → HASH TABLE
├─ Sorted data with range queries → TREE
└─ Graph/Tree structure → GRAPH ALGORITHMS
```

### SLIDING WINDOW

```
Window size?
├─ Fixed size
│   ├─ Need max/min in window? → DEQUE-BASED WINDOW
│   └─ Need sum/avg? → SIMPLE SLIDING WINDOW
├─ Variable size
│   ├─ Based on distinct elements? → FREQUENCY MAP WINDOW
│   ├─ Based on condition? → EXPAND/SHRINK WINDOW
│   └─ Based on pattern? → FREQUENCY MAP + EXPAND/SHRINK
└─ Time-based
    ├─ Fixed time window? → TIME-BASED RING BUFFER (Linux kfifo)
    └─ Variable time window? → TIME-BASED DEQUE
```

### HASH TABLE

```
Concurrency / Use case?
├─ High concurrency, many readers? → LINUX KERNEL LOCK-FREE (RCU)
├─ Need O(1) worst-case lookup? → CUCKOO HASHING
├─ Need non-blocking resize? → REDIS OPEN ADDRESSING (Incremental Rehashing)
├─ Memory efficiency critical? → POSTGRESQL CHAINING
├─ Want better cache performance? → ROBIN HOOD HASHING
└─ Standard use case → POSTGRESQL CHAINING or REDIS OPEN ADDRESSING
```

### TREE

```
Data fits in memory?
├─ YES
│   ├─ Need generic/intrusive? → LINUX KERNEL RED-BLACK TREE
│   ├─ Want simpler implementation? → LEFT-LEANING RED-BLACK TREE
│   └─ Standard use case → LEFT-LEANING RED-BLACK TREE
└─ NO (disk-based)
    └─ POSTGRESQL B-TREE
```

### GRAPH ALGORITHMS

```
Graph type / Use case?
├─ Tree/hierarchy traversal with scheduling? → REACT FIBER RECONCILIATION
├─ Control flow analysis / compiler? → LLVM CONTROL FLOW GRAPH
├─ Need dominator information? → DOMINATOR TREE ALGORITHM
└─ General graph traversal? → REACT FIBER or standard DFS/BFS
```

### TWO POINTERS

```
Data structure?
├─ Sorted Array
│   ├─ Need pairs/triplets? → OPPOSITE ENDS
│   ├─ Remove duplicates? → SAME DIRECTION
│   └─ Merge/intersection? → MERGE PATTERN
├─ Linked List
│   ├─ Cycle detection? → FAST/SLOW (Floyd)
│   ├─ Find middle? → FAST/SLOW
│   └─ Reverse/partition? → TWO POINTERS
└─ Unsorted Array/String
    ├─ Variable window? → SLIDING WINDOW (two pointers)
    └─ In-place modification? → SAME DIRECTION
```

### K-WAY MERGE

```
How many sequences?
├─ 2 sequences → TWO POINTERS MERGE
├─ K sequences (small K) → HEAP-BASED MERGE
└─ K sequences (large K) → DIVIDE-AND-CONQUER MERGE
```

---

## TRANSFORM Problems

### Step 1: What Transformation?

```
Transformation type?
├─ Sort → SORTING ALGORITHMS
├─ Filter/Remove → TWO POINTERS (same direction)
├─ Partition → TWO POINTERS
└─ Reverse → TWO POINTERS
```

---

## Quick Reference: Pattern → Variant Mapping

### Hash Tables

| Input | Variant | Source |
|-------|---------|--------|
| High concurrency, many readers | Linux Kernel Lock-Free | Linux kernel |
| O(1) worst-case lookup | Cuckoo Hashing | Research papers |
| Non-blocking resize | Redis Open Addressing | Redis |
| Memory efficiency | PostgreSQL Chaining | PostgreSQL |
| Better cache performance | Robin Hood Hashing | Research papers |
| Standard use case | PostgreSQL Chaining | PostgreSQL |

### Trees

| Input | Variant | Source |
|-------|---------|--------|
| Generic/intrusive needed | Linux Kernel Red-Black | Linux kernel |
| Simpler implementation | Left-Leaning Red-Black | Research papers |
| Large dataset, disk-based | PostgreSQL B-Tree | PostgreSQL |
| Standard use case | Left-Leaning Red-Black | Research papers |

### Graph Algorithms

| Input | Variant | Source |
|-------|---------|--------|
| Tree traversal with scheduling | React Fiber | React |
| Control flow analysis | LLVM CFG | LLVM |
| Dominator information | Dominator Tree | Research papers |
| General traversal | React Fiber | React |

### Binary Search

| Input | Variant | Source |
|-------|---------|--------|
| Sorted, size ≤ 8 | Linear search | V8 |
| Sorted by hash | Hash-based binary | V8 |
| Need insertion point | Hybrid binary+linear | ICU |
| Large array (overflow risk) | Overflow-safe | V8 CodeGen |
| Standard sorted | Standard binary | Generic |

### Sliding Window

| Input | Variant | Source |
|-------|---------|--------|
| Fixed size, sum/avg | Simple sliding | Generic |
| Fixed size, max/min | Deque-based | Generic |
| Variable size, distinct | Frequency map | Generic |
| Time-based, lock-free | Linux kfifo | Linux kernel |
| Compression lookback | Brotli ring buffer | Brotli |
| Small metrics | V8 simple ring | V8 |

### Two Pointers

| Input | Variant | Source |
|-------|---------|--------|
| Sorted array, pairs | Opposite ends | Generic |
| Linked list, cycle | Fast/slow | Floyd |
| Remove duplicates | Same direction | Generic |
| Variable window | Sliding window | Generic |
| Merge sequences | Merge pattern | Generic |

### K-way Merge

| Input | Variant | Source |
|-------|---------|--------|
| K=2 | Two pointers | Generic |
| Small K | Heap-based | Generic |
| Large K | Divide-and-conquer | Generic |
| External data | Streaming | Generic |

---

## Decision Flow Examples

### Example 1: High-Performance Cache

```
Problem: Need fast cache with non-blocking operations
├─ Key-value storage? YES
├─ Hash table needed? YES
├─ Non-blocking resize? YES
└─ Result: Redis Open Addressing
```

### Example 2: Compiler Symbol Table

```
Problem: Fast symbol lookup with O(1) worst-case
├─ Key-value storage? YES
├─ Hash table needed? YES
├─ O(1) worst-case? YES
└─ Result: Cuckoo Hashing
```

### Example 3: Database Index

```
Problem: Large dataset index
├─ Sorted data? YES
├─ Range queries? YES
├─ Data fits in memory? NO
└─ Result: PostgreSQL B-Tree
```

### Example 4: UI Rendering

```
Problem: Component tree reconciliation
├─ Tree structure? YES
├─ Need scheduling? YES
├─ Incremental processing? YES
└─ Result: React Fiber Reconciliation
```

### Example 5: Compiler Optimization

```
Problem: Need dominator information
├─ Graph structure? YES
├─ Control flow? YES
├─ Dominator needed? YES
└─ Result: Dominator Tree Algorithm
```

---

## Pattern Recognition Checklist

Before choosing an algorithm, ask:

1. **What am I trying to do?** (Search, Process, Transform)
2. **What's the data structure?** (Array, Linked List, Tree, Graph, Hash Table)
3. **Is data sorted?** (Yes/No/Can sort)
4. **What are the constraints?** (Size, time, space, concurrency)
5. **What's the operation?** (Find, compute, modify)
6. **What's the use case?** (Cache, database, compiler, UI, etc.)

---

## Universal Applications

Each pattern has universal applications beyond LeetCode:

- **Hash Tables**: Caching, databases, symbol tables, key-value storage
- **Trees**: Databases, file systems, sorted data, range queries
- **Graph Algorithms**: Compilers, UI rendering, network analysis, program understanding
- **Binary Search**: Version control, UI rendering, game engines, database indexing
- **Sliding Window**: Rate limiting, network packet analysis, log analysis, compression
- **Two Pointers**: Linked list operations, array partitioning, string processing
- **K-way Merge**: External sorting, database merge joins, log merging

---

## Next Steps

1. Identify problem characteristics
2. Follow decision tree
3. Select appropriate variant
4. Consider production optimizations
5. Implement with real-world considerations

