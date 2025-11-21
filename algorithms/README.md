# Production Pattern Recognition System

## Goal

Extract ingenious, hacky, god-mode algorithmic techniques from **REAL production codebases** (Node.js, Linux kernel, React, Redis, etc.), document ALL variants, and build pattern recognition for when to apply them in production code.

**NOT LeetCode problems** - Real implementations from production systems.

## Structure

```
algorithms/
├── production_patterns/           # Extracted from real codebases
│   ├── binary_search/             # Binary search variants
│   │   ├── variants/             # All implementation variants
│   │   │   ├── standard.cpp
│   │   │   ├── v8_hash_based.cpp
│   │   │   ├── icu_hybrid.cpp
│   │   │   ├── linux_kernel.cpp
│   │   │   └── v8_overflow_safe.cpp
│   │   └── PATTERN_RECOGNITION.md
│   ├── sliding_window/            # Sliding window variants
│   │   ├── variants/
│   │   │   ├── linux_kfifo.cpp
│   │   │   ├── brotli_ring_buffer.cpp
│   │   │   └── v8_simple_ring_buffer.cpp
│   │   └── PATTERN_RECOGNITION.md
│   ├── two_pointers/             # Two pointers variants
│   │   └── PATTERN_RECOGNITION.md
│   └── k_way_merge/              # K-way merge variants
│       └── PATTERN_RECOGNITION.md
├── extraction_notes/              # Notes from analyzing codebases
│   ├── NODE_V8_ANALYSIS.md
│   └── LINUX_KERNEL_ANALYSIS.md
├── PRODUCTION_PATTERN_RECOGNITION.md  # Main methodology
└── PATTERN_DECISION_TREE.md      # Master decision tree
```

## Quick Start

1. **Identify Problem Characteristics**: Use `PATTERN_DECISION_TREE.md`
2. **Select Variant**: Choose appropriate variant from pattern directory
3. **Review Real-World Examples**: See where it's used in production
4. **Implement**: Use production-grade variant

## Extraction Sources

We extract patterns from **multiple sources**:
- **Local Codebases**: Linux kernel, Node.js/V8
- **GitHub Repositories**: Redis, PostgreSQL, nginx, React, MongoDB, etc.
- **Research Papers**: ACM, IEEE, arXiv
- **Technical Blogs**: Engineering blogs from major tech companies

See `SOURCE_TRACKING.md` for complete source list.

## Patterns Extracted

### Binary Search (6 Variants)
- **Standard**: Generic binary search
- **V8 Hash-Based**: Hash comparison + linear scan for collisions
- **ICU Hybrid**: Binary until small, then linear
- **V8 Overflow-Safe**: Conditional mid calculation
- **V8 Small Array**: Linear for ≤8 elements

**Sources**: V8, ICU, Linux kernel

### Sliding Window (3 Variants)
- **Linux kfifo**: Lock-free ring buffer, power-of-2 optimization
- **Brotli Ring Buffer**: Tail duplication for compression
- **V8 Simple Ring Buffer**: Constexpr, small fixed-size

**Sources**: Linux kernel, Brotli, V8

### Two Pointers (5 Variants)
- **Opposite Ends**: Sorted array, pairs/triplets
- **Fast/Slow**: Linked list, cycle detection
- **Same Direction**: Remove duplicates
- **Sliding Window**: Variable size window
- **Merge Pattern**: Two sorted sequences

**Sources**: Generic patterns, Floyd's algorithm

### K-way Merge (4 Variants)
- **Two Pointers**: K=2 merge
- **Heap-Based**: Small K, priority queue
- **Divide-and-Conquer**: Large K, recursive
- **Streaming**: External sort, doesn't fit in memory

**Sources**: Generic patterns, external sorting

## Key Principles

1. **Extract from Production**: Real codebases, not LeetCode
2. **Document Variants**: All ways to implement, including ingenious ones
3. **Pattern Recognition**: Know WHEN to use each variant
4. **Real-World Examples**: Where it's actually used in production

## Usage

### For Binary Search:
```cpp
// Check PATTERN_DECOGNITION.md for variant selection
// Then use appropriate variant from variants/ directory
```

### For Sliding Window:
```cpp
// Check PATTERN_RECOGNITION.md for variant selection
// Then use appropriate variant from variants/ directory
```

## Next Steps

1. Extract hash tables from Redis, PostgreSQL, Linux kernel, research papers
2. Extract tree patterns (red-black trees, B-trees) from multiple sources
3. Extract graph algorithms from React, LLVM, research papers
4. Expand source coverage (more GitHub repos, more research papers)
5. Build interactive decision tools
6. Create comprehensive pattern library

## Extraction Methodology

See `EXTRACTION_METHODOLOGY.md` for detailed multi-source extraction process.
