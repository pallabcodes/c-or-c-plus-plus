# Binary Search Pattern Recognition

## When to Recognize Binary Search Opportunity

### Input Characteristics That Suggest Binary Search

1. **Sorted Data**
   - Array is sorted (ascending/descending)
   - Data structure maintains sorted order
   - Can be sorted with O(n log n) preprocessing

2. **Search Operation**
   - Finding element
   - Finding insertion point
   - Finding boundary (first/last occurrence)
   - Finding range

3. **Monotonic Property**
   - Function is monotonic (increasing/decreasing)
   - Can use binary search on answer space
   - Example: Finding minimum/maximum satisfying condition

### Problem Patterns That Use Binary Search

1. **Direct Search**
   - "Find X in sorted array"
   - "Find insertion point for X"
   - "Find first/last occurrence of X"

2. **Binary Search on Answer**
   - "Find minimum/maximum X such that condition holds"
   - "Find optimal value"
   - Example: "Find minimum time to complete task"

3. **Range Queries**
   - "Find all elements in range [L, R]"
   - "Count elements in range"

4. **Rotated/Modified Arrays**
   - "Search in rotated sorted array"
   - "Find peak in mountain array"

## Variant Selection Guide

### Standard Binary Search

**Use When**:
- Simple sorted array search
- No special requirements
- Array size > 8

**Code Pattern**:
```cpp
int left = 0, right = n - 1;
while (left <= right) {
    int mid = left + (right - left) / 2;
    if (arr[mid] == target) return mid;
    else if (arr[mid] < target) left = mid + 1;
    else right = mid - 1;
}
```

### Hash-Based Binary Search (V8 Variant)

**Use When**:
- Searching by computed key (hash, checksum, etc.)
- Key comparison is expensive
- Hash collisions possible

**Code Pattern**:
```cpp
// Binary search on hash
int pos = binary_search_by_hash(hash);
// Linear scan for collisions
for (; pos < end && hash_match(pos); pos++) {
    if (exact_match(pos)) return pos;
}
```

**Real-World Example**: V8 object property lookup

### Hybrid Binary + Linear (ICU Variant)

**Use When**:
- Finding insertion point
- Small sub-arrays after binary phase
- Need stable sort insertion point

**Code Pattern**:
```cpp
// Binary search until small
while (range_size >= MIN_THRESHOLD) {
    int mid = (start + end) / 2;
    // ... binary search logic
}
// Linear search for small range
while (start < end) {
    // ... linear search
}
```

**Real-World Example**: ICU stable sorting

### Overflow-Safe Variant (V8 CodeGen)

**Use When**:
- Array size may exceed INT_MAX/2
- Code generation / compiler backends
- Need overflow protection

**Code Pattern**:
```cpp
int mid = (max_size < INT_MAX/2) 
    ? (low + high) / 2           // Fast path
    : low + (high - low) / 2;    // Safe path
```

**Real-World Example**: V8 TurboFan compiler

### Small Array Optimization (V8 Variant)

**Use When**:
- Array size ≤ 8
- Linear search is faster (cache-friendly)
- Avoiding binary search overhead

**Code Pattern**:
```cpp
if (size <= 8) {
    return linear_search(arr, size, target);
} else {
    return binary_search(arr, size, target);
}
```

**Real-World Example**: V8 DescriptorArray (≤8 elements)

## Pattern Recognition Checklist

### Before Implementing Binary Search, Ask:

1. **Is data sorted?**
   - ✅ YES → Consider binary search
   - ❌ NO → Can it be sorted? Is sorting worth it?

2. **What am I searching for?**
   - Element existence → Standard binary search
   - Insertion point → Hybrid variant (ICU)
   - First/last occurrence → Modified binary search
   - Range → Binary search boundaries

3. **What are the constraints?**
   - Small array (≤8) → Linear search (V8)
   - Large array (overflow risk) → Overflow-safe (V8 CodeGen)
   - Hash-based key → Hash binary search (V8)
   - Need stable insertion → Hybrid (ICU)

4. **What's the comparison cost?**
   - Cheap (integers) → Standard binary search
   - Expensive (strings) → Hash-based (V8)
   - Custom comparator → Generic binary search (Linux)

## Common Mistakes to Avoid

1. **Using binary search on unsorted data**
   - Always verify data is sorted
   - Or sort first (if worth it)

2. **Not handling edge cases**
   - Empty array
   - Single element
   - All elements same
   - Target not found

3. **Overflow in mid calculation**
   - Use `left + (right - left) / 2` for large arrays
   - Or use overflow-safe variant (V8 CodeGen)

4. **Not optimizing for small arrays**
   - Linear search is faster for ≤8 elements (V8 approach)
   - Cache-friendly

5. **Not considering variant selection**
   - Hash-based for expensive comparisons
   - Hybrid for insertion points
   - Overflow-safe for large arrays

## Real-World Recognition Examples

### Example 1: Version Control (Git Bisect)

**Problem**: Find commit that introduced bug

**Pattern Recognition**:
- Commits are ordered (by time)
- Need to find boundary (good → bad)
- Binary search on commit range

**Variant**: Modified binary search (find boundary)

### Example 2: UI Rendering (React Virtualization)

**Problem**: Find which items are visible in viewport

**Pattern Recognition**:
- Items are sorted by position
- Need to find range [start_y, end_y]
- Binary search for start and end positions

**Variant**: Binary search for boundaries

### Example 3: Game Engine (Collision Detection)

**Problem**: Find objects in spatial region

**Pattern Recognition**:
- Objects sorted by position
- Need range query
- Binary search for region boundaries

**Variant**: Binary search for range boundaries

### Example 4: Database Index Lookup

**Problem**: Find records by indexed key

**Pattern Recognition**:
- Index is sorted by key
- Need fast lookup
- May use hash-based if key is expensive to compare

**Variant**: Standard or hash-based binary search

## Decision Tree

```
Need to search?
├─ Data sorted?
│  ├─ NO → Can sort? → Sort + binary search OR linear search
│  └─ YES
│     ├─ Size ≤ 8? → Linear search (V8 optimization)
│     ├─ Searching by hash? → Hash-based binary search (V8)
│     ├─ Need insertion point? → Hybrid binary+linear (ICU)
│     ├─ Size > INT_MAX/2? → Overflow-safe variant (V8 CodeGen)
│     └─ Standard → Standard binary search
└─ Monotonic function?
   └─ YES → Binary search on answer space
```

