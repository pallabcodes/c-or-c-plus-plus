# Tree Pattern Recognition

## When to Recognize Tree Opportunity

### Input Characteristics That Suggest Tree

1. **Sorted Data with Range Queries**
   - Need to maintain sorted order
   - Range queries (find all keys in [a, b])
   - Need predecessor/successor operations

2. **O(log n) Operations Required**
   - Need guaranteed O(log n) operations
   - Balanced tree structure needed
   - Avoid worst-case O(n) performance

3. **Dynamic Data Set**
   - Frequent insertions/deletions
   - Data changes over time
   - Need to maintain balance

4. **Hierarchical Data**
   - Tree-structured data naturally
   - Parent-child relationships
   - Hierarchical queries

## Variant Selection Guide

### Decision Tree

```
Need tree data structure?
├─ Data fits in memory?
│  ├─ YES
│  │  ├─ Need generic/intrusive?
│  │  │  └─ YES → Linux Kernel Red-Black Tree
│  │  ├─ Want simpler implementation?
│  │  │  └─ YES → Left-Leaning Red-Black Tree
│  │  └─ Standard use case
│  │     └─ Left-Leaning Red-Black Tree
│  └─ NO (disk-based)
│     └─ PostgreSQL B-Tree
```

### Red-Black Tree vs B-Tree

**Red-Black Tree**:
- **Pros**: In-memory, simple structure, good for general use
- **Cons**: Lower fan-out, deeper trees
- **Use When**: Data fits in memory, general-purpose balanced BST

**B-Tree**:
- **Pros**: High fan-out, shallow trees, disk-optimized
- **Cons**: More complex, designed for disk
- **Use When**: Large datasets, disk-based storage, database indexes

### Specific Variant Selection

#### Linux Kernel Red-Black Tree

**Use When**:
- Need generic/intrusive tree implementation
- Memory efficiency critical
- Kernel-level code
- High-performance systems

**Key Features**:
- Intrusive data structures
- Parent + color packed in single field
- RCU support for lock-free reads
- Generic type-agnostic

**Trade-offs**:
- More complex implementation
- Requires understanding intrusive patterns
- Better for kernel/system code

#### Left-Leaning Red-Black Tree

**Use When**:
- Want simpler red-black tree implementation
- Educational purposes
- Code clarity important
- Standard balanced BST needs

**Key Features**:
- Simplified implementation (2 cases vs 3)
- Red nodes only as left children
- Easier to implement correctly
- Same O(log n) guarantees

**Trade-offs**:
- Slightly less optimized
- Easier to understand and maintain
- Good for general-purpose use

#### PostgreSQL B-Tree

**Use When**:
- Large datasets (don't fit in memory)
- Database indexes
- Disk-based storage
- Range queries important

**Key Features**:
- Disk-based structure (pages)
- High fan-out (many keys per node)
- MVCC for concurrency
- Split/merge operations

**Trade-offs**:
- More complex than in-memory trees
- Designed for disk I/O
- Better for large datasets

## Input Characteristics → Variant Mapping

| Input Characteristic | Recommended Variant | Reason |
|---------------------|---------------------|--------|
| Data fits in memory, generic needed | Linux Kernel Red-Black | Intrusive, generic |
| Data fits in memory, simple needed | Left-Leaning Red-Black | Simpler implementation |
| Large dataset, disk-based | PostgreSQL B-Tree | Disk-optimized |
| Database index | PostgreSQL B-Tree | Standard for databases |
| Kernel/system code | Linux Kernel Red-Black | Intrusive, RCU support |
| Educational/learning | Left-Leaning Red-Black | Easier to understand |

## Performance Characteristics Comparison

| Variant | Insert | Search | Delete | Memory | Best For |
|---------|--------|--------|--------|--------|----------|
| Linux Kernel Red-Black | O(log n) | O(log n) | O(log n) | O(n) | Generic, intrusive |
| Left-Leaning Red-Black | O(log n) | O(log n) | O(log n) | O(n) | Simple, general |
| PostgreSQL B-Tree | O(log n) | O(log n) | O(log n) | O(n) | Disk-based, large |

## Real-World Examples

### Linux Kernel Red-Black Tree
- **Process Scheduler**: Process priority queues
- **Virtual Memory**: VMA (Virtual Memory Area) management
- **I/O Schedulers**: Request queues

### Left-Leaning Red-Black Tree
- **Java TreeMap**: Similar simplified approach
- **Educational Implementations**: Learning balanced trees
- **General-Purpose**: Standard balanced BST needs

### PostgreSQL B-Tree
- **PostgreSQL Indexes**: Default index type
- **Database Storage**: Primary indexing mechanism
- **File Systems**: Directory structures

## Pattern Recognition Checklist

Before choosing a tree variant, ask:

1. **Does data fit in memory?**
   - YES → Red-Black Tree
   - NO → B-Tree

2. **Need generic/intrusive implementation?**
   - YES → Linux Kernel Red-Black
   - NO → Left-Leaning Red-Black

3. **Want simpler implementation?**
   - YES → Left-Leaning Red-Black
   - NO → Linux Kernel Red-Black

4. **Is it for database/index?**
   - YES → PostgreSQL B-Tree
   - NO → Red-Black Tree

5. **Need range queries?**
   - YES → Any variant (all support)
   - NO → Any variant

## Common Mistakes to Avoid

1. **Using B-Tree for in-memory data**
   - B-Tree overhead unnecessary
   - Red-Black Tree more efficient

2. **Using Red-Black Tree for disk-based data**
   - Poor disk I/O characteristics
   - B-Tree designed for disk

3. **Not considering simplicity**
   - Complex implementation when simple suffices
   - Left-leaning easier to maintain

4. **Ignoring memory constraints**
   - Using in-memory tree for large data
   - Should use B-Tree for disk

5. **Wrong tree for use case**
   - Generic tree when specific needed
   - Simple tree when generic needed

## Universal Applications

- **Red-Black Trees**: In-memory sorted data, balanced BST, general-purpose
- **B-Trees**: Databases, file systems, large datasets, disk-based storage

