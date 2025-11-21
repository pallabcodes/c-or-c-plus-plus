# Tree Extraction Notes

## Summary

Extracted 3 tree variants from multiple sources:
- **Linux Kernel** (Local): Generic intrusive red-black tree
- **PostgreSQL** (GitHub): Disk-based B-tree
- **Left-Leaning Red-Black** (Research): Simplified red-black tree

## Extracted Variants

### 1. Linux Kernel Red-Black Tree

**Source**: `/Users/picon/Learning/c-or-c-plus-plus/linux/include/linux/rbtree.h`
**File**: `linux/include/linux/rbtree.h`
**Implementation**: `linux/lib/rbtree.c`
**Variant File**: `production_patterns/red_black_tree/variants/linux_kernel.cpp`

**Key Features**:
- Intrusive data structures (rb_node embedded in containing structure)
- Parent pointer + color packed in single field (__rb_parent_color)
- Generic type-agnostic implementation
- RCU support for lock-free reads
- Leftmost caching for O(1) minimum

**Key Insights**:
- Intrusive pattern eliminates extra allocations
- Packing parent+color saves memory
- Generic implementation avoids callbacks for performance
- RCU enables lock-free reads
- Three-case insertion fixup

**Performance Characteristics**:
- Insert: O(log n)
- Search: O(log n)
- Delete: O(log n)
- Space: O(n)

**Use Cases**:
- Kernel-level code
- High-performance systems
- Memory-efficient trees
- Generic tree needs

### 2. PostgreSQL B-Tree

**Source**: https://github.com/postgres/postgres/blob/master/src/backend/access/nbtree/
**Repository**: postgres/postgres
**Directory**: `src/backend/access/nbtree/`
**Variant File**: `production_patterns/b_tree/variants/postgresql.cpp`

**Key Features**:
- Disk-based structure (pages instead of nodes)
- High fan-out (many keys per node)
- Split/merge operations for balance
- MVCC (Multi-Version Concurrency Control)
- Page management for disk I/O

**Key Insights**:
- Pages optimize disk I/O (read/write entire pages)
- High fan-out creates shallow trees
- Split/merge maintains balance
- MVCC enables concurrent access
- Designed for large datasets

**Performance Characteristics**:
- Insert: O(log n)
- Search: O(log n)
- Delete: O(log n)
- Range Query: O(log n + k) where k is result size

**Use Cases**:
- Database indexes
- Large datasets
- Disk-based storage
- Range queries

### 3. Left-Leaning Red-Black Tree

**Source**: "Left-Leaning Red-Black Trees" by Robert Sedgewick
**Paper**: Various papers and presentations
**Variant File**: `production_patterns/red_black_tree/variants/left_leaning.cpp`

**Key Features**:
- Simplified implementation (2 cases vs 3)
- Red nodes only as left children
- Easier to implement correctly
- Same O(log n) guarantees

**Key Insights**:
- Left-leaning invariant reduces cases
- Simpler code is easier to verify
- Educational value
- Same performance as standard red-black

**Performance Characteristics**:
- Insert: O(log n)
- Search: O(log n)
- Delete: O(log n)
- Space: O(n)

**Use Cases**:
- Educational implementations
- General-purpose balanced BST
- When code clarity matters
- Standard tree needs

## Comparison of Variants

### Structure

| Variant | Structure | Memory | Complexity |
|---------|-----------|--------|------------|
| Linux Kernel | Intrusive nodes | Efficient | High |
| PostgreSQL | Disk pages | Disk-optimized | High |
| Left-Leaning | Standard nodes | Standard | Low |

### Use Cases

| Variant | Best For | Worst For |
|---------|----------|-----------|
| Linux Kernel | Generic, intrusive, kernel | Simple use cases |
| PostgreSQL | Large datasets, disk | In-memory small data |
| Left-Leaning | General-purpose, simple | Complex requirements |

## Key Insights from Sources

### Linux Kernel Insights
1. **Intrusive Structures**: Eliminate extra allocations
2. **Packed Fields**: Save memory with bit manipulation
3. **Generic Design**: Avoid callbacks for performance
4. **RCU Support**: Enable lock-free reads

### PostgreSQL Insights
1. **Page-Based**: Optimize disk I/O
2. **High Fan-Out**: Shallow trees for better performance
3. **MVCC**: Enable concurrent access
4. **Split/Merge**: Maintain balance efficiently

### Left-Leaning Insights
1. **Simplification**: Fewer cases = easier to implement
2. **Left-Leaning Invariant**: Reduces complexity
3. **Educational Value**: Easier to understand
4. **Same Guarantees**: Performance not sacrificed

## Performance Summary

| Metric | Linux Kernel | PostgreSQL | Left-Leaning |
|--------|--------------|------------|--------------|
| Insert | O(log n) | O(log n) | O(log n) |
| Search | O(log n) | O(log n) | O(log n) |
| Delete | O(log n) | O(log n) | O(log n) |
| Memory | Efficient | Disk-optimized | Standard |
| Complexity | High | High | Low |

## Use Case Recommendations

### Choose Linux Kernel Red-Black When:
- Need generic/intrusive implementation
- Memory efficiency critical
- Kernel/system code
- High-performance requirements

### Choose PostgreSQL B-Tree When:
- Large datasets (don't fit in memory)
- Database indexes
- Disk-based storage
- Range queries important

### Choose Left-Leaning Red-Black When:
- Want simpler implementation
- Educational purposes
- Code clarity important
- General-purpose balanced BST

