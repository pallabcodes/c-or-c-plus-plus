# K-way Merge Pattern Recognition

## When to Recognize K-way Merge Opportunity

### Input Characteristics That Suggest K-way Merge

1. **Multiple Sorted Sequences**
   - K sorted arrays/lists
   - Need to merge into one sorted sequence
   - Need to find smallest/largest across sequences

2. **External Sorting**
   - Data doesn't fit in memory
   - Need to merge sorted chunks
   - Streaming/iterative processing

3. **Priority Queue Operations**
   - Need to maintain sorted order across K sources
   - Need to repeatedly get minimum/maximum
   - Need to merge results from multiple sources

### Problem Patterns That Use K-way Merge

1. **Merge K Sorted Lists**
   - K linked lists, each sorted
   - Merge into one sorted list

2. **Merge K Sorted Arrays**
   - K arrays, each sorted
   - Merge into one sorted array

3. **Find Kth Smallest in K Sorted Arrays**
   - K sorted arrays
   - Find Kth smallest element overall

4. **External Sort Merge Phase**
   - Merge sorted runs from disk
   - Streaming merge

## Variant Selection Guide

### Two Sequences (K=2)

**Use When**:
- Only 2 sorted sequences
- Standard merge operation

**Code Pattern**:
```cpp
int i = 0, j = 0;
while (i < n && j < m) {
    if (arr1[i] <= arr2[j]) {
        result[k++] = arr1[i++];
    } else {
        result[k++] = arr2[j++];
    }
}
// Copy remaining
```

**Time Complexity**: O(n + m)
**Space Complexity**: O(n + m) for result array

**Real-World Example**: Merge two sorted arrays, merge two sorted lists

### Heap-Based Merge (Small K)

**Use When**:
- K sorted sequences (K is small, e.g., K < 100)
- Need to merge efficiently
- Can use priority queue

**Code Pattern**:
```cpp
priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;

// Initialize with first element from each list
for (int i = 0; i < k; i++) {
    if (!lists[i].empty()) {
        pq.push({lists[i][0], i});
    }
}

while (!pq.empty()) {
    auto [val, list_idx] = pq.top();
    pq.pop();
    result.push_back(val);
    
    // Add next element from same list
    if (lists[list_idx].has_next()) {
        pq.push({lists[list_idx].next(), list_idx});
    }
}
```

**Time Complexity**: O(N log K) where N is total elements
**Space Complexity**: O(K) for heap

**Real-World Example**: Merge K sorted lists, find Kth smallest across K arrays

### Divide-and-Conquer Merge (Large K)

**Use When**:
- K is large (K > 100)
- Want to reduce heap overhead
- Can merge recursively

**Code Pattern**:
```cpp
vector<int> mergeKLists(vector<vector<int>>& lists, int left, int right) {
    if (left == right) return lists[left];
    
    int mid = (left + right) / 2;
    vector<int> left_merged = mergeKLists(lists, left, mid);
    vector<int> right_merged = mergeKLists(lists, mid + 1, right);
    
    return mergeTwoLists(left_merged, right_merged);
}
```

**Time Complexity**: O(N log K) - same as heap, but better constant factors
**Space Complexity**: O(N) for recursion stack and intermediate results

**Real-World Example**: External sorting, large-scale data merging

### Streaming Merge (External Sort)

**Use When**:
- Data doesn't fit in memory
- Need to merge sorted runs from disk
- Streaming/iterative processing

**Code Pattern**:
```cpp
// Use min-heap with iterators/file pointers
priority_queue<Iterator, vector<Iterator>, Compare> pq;

// Initialize with first element from each run
for (auto& run : runs) {
    if (run.has_next()) {
        pq.push(run.iterator());
    }
}

while (!pq.empty()) {
    auto it = pq.top();
    pq.pop();
    
    output.write(it.value());
    
    if (it.has_next()) {
        it.advance();
        pq.push(it);
    }
}
```

**Time Complexity**: O(N log K)
**Space Complexity**: O(K) - only K elements in memory

**Real-World Example**: External sorting, database merge joins, log file merging

## Pattern Recognition Checklist

### Before Implementing K-way Merge, Ask:

1. **How many sequences?**
   - K = 2 → Two pointers merge
   - K small (< 100) → Heap-based merge
   - K large (> 100) → Divide-and-conquer merge

2. **Does data fit in memory?**
   - YES → Heap-based or divide-and-conquer
   - NO → Streaming merge (external sort)

3. **What's the access pattern?**
   - Random access → Heap-based
   - Sequential access → Streaming merge
   - Can be sorted → Divide-and-conquer

4. **What's the output requirement?**
   - Full merged result → All variants work
   - Kth smallest only → Heap-based (stop early)
   - Streaming output → Streaming merge

5. **Performance requirements?**
   - Maximum speed → Divide-and-conquer (better cache)
   - Minimal memory → Streaming merge
   - Simple implementation → Heap-based

## Common Mistakes to Avoid

1. **Using heap for K=2**
   - Two pointers merge is simpler and faster
   - Heap adds unnecessary overhead

2. **Not handling empty sequences**
   - Check if sequences are empty
   - Handle edge cases

3. **Inefficient heap operations**
   - Use min-heap for ascending order
   - Use max-heap for descending order
   - Don't push entire sequences into heap

4. **Not considering memory constraints**
   - For large K, use streaming merge
   - Don't load all data into memory

5. **Wrong comparator**
   - Ensure heap maintains correct order
   - Test with different data types

## Real-World Recognition Examples

### Example 1: Merge K Sorted Lists (LeetCode)

**Problem**: Merge K sorted linked lists

**Pattern Recognition**:
- K sorted sequences
- K is typically small (< 100)
- Need full merged result

**Variant**: Heap-based merge

### Example 2: External Sorting

**Problem**: Sort large file that doesn't fit in memory

**Pattern Recognition**:
- Data doesn't fit in memory
- Need to merge sorted runs
- Streaming output

**Variant**: Streaming merge (external sort)

### Example 3: Database Merge Join

**Problem**: Join two sorted tables

**Pattern Recognition**:
- Two sorted sequences
- Need to merge with join condition
- May not fit in memory

**Variant**: Two pointers merge or streaming merge

### Example 4: Log File Merging

**Problem**: Merge multiple sorted log files

**Pattern Recognition**:
- K sorted files
- May be large (don't fit in memory)
- Need streaming output

**Variant**: Streaming merge

## Decision Tree

```
Need to merge K sorted sequences?
├─ K = 2?
│  └─ YES → Two pointers merge
├─ Data fits in memory?
│  ├─ YES
│  │  ├─ K small (< 100)? → Heap-based merge
│  │  └─ K large (> 100)? → Divide-and-conquer merge
│  └─ NO → Streaming merge (external sort)
└─ Need only Kth element?
   └─ YES → Heap-based merge (stop early)
```

## Performance Comparison

| Variant | Time | Space | Best For |
|---------|------|-------|----------|
| Two Pointers | O(n+m) | O(n+m) | K=2 |
| Heap-Based | O(N log K) | O(K) | Small K, random access |
| Divide-and-Conquer | O(N log K) | O(N) | Large K, better cache |
| Streaming | O(N log K) | O(K) | External data, streaming |

## Universal Applications

1. **External Sorting**: Merge sorted runs from disk
2. **Database Systems**: Merge joins, union operations
3. **Log Processing**: Merge sorted log files
4. **Search Engines**: Merge inverted index postings
5. **Distributed Systems**: Merge results from multiple nodes


