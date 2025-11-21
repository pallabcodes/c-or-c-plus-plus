# Two Pointers Pattern Recognition

## When to Recognize Two Pointers Opportunity

### Input Characteristics That Suggest Two Pointers

1. **Sorted Array**
   - Array is sorted (ascending/descending)
   - Need to find pairs/triplets with properties
   - Need to find boundaries or ranges

2. **Linked List**
   - Need to find middle, cycle, or nth from end
   - Need to reverse or partition
   - Need to merge two lists

3. **String/Array with Constraints**
   - Need to find substrings/subarrays
   - Need to remove duplicates
   - Need to partition based on condition

4. **Two Sequences**
   - Two sorted arrays/lists
   - Need to merge or find intersection
   - Need to compare elements

### Problem Patterns That Use Two Pointers

1. **Opposite Ends (Sorted Array)**
   - Two pointers from start and end
   - Move towards each other
   - Example: Pair with target sum, palindrome check

2. **Same Direction (Fast/Slow)**
   - Two pointers moving in same direction
   - Different speeds (fast moves 2x, slow moves 1x)
   - Example: Cycle detection, find middle, remove duplicates

3. **Sliding Window Variant**
   - Two pointers define window boundaries
   - Expand/shrink window
   - Example: Longest substring, minimum window

4. **Merge Pattern**
   - Two pointers in different sequences
   - Merge sorted arrays/lists
   - Example: Merge two sorted lists, find intersection

## Variant Selection Guide

### Opposite Ends Pattern

**Use When**:
- Array is sorted
- Need to find pairs with properties
- Can eliminate half of search space

**Code Pattern**:
```cpp
int left = 0, right = n - 1;
while (left < right) {
    if (condition) {
        // Process or return
    } else if (arr[left] + arr[right] < target) {
        left++; // Need larger sum
    } else {
        right--; // Need smaller sum
    }
}
```

**Real-World Example**: Pair with target sum, 3Sum, container with most water

### Fast/Slow Pointers (Floyd's Cycle Detection)

**Use When**:
- Linked list cycle detection
- Find middle of list
- Find nth node from end
- Remove duplicates in sorted array

**Code Pattern**:
```cpp
ListNode* slow = head;
ListNode* fast = head;

while (fast && fast->next) {
    slow = slow->next;      // Move 1 step
    fast = fast->next->next; // Move 2 steps
    
    if (slow == fast) {
        // Cycle detected
    }
}
```

**Real-World Example**: Cycle detection, find middle, palindrome check

### Same Direction (Remove Duplicates)

**Use When**:
- Remove duplicates in sorted array
- In-place modifications
- Need to maintain relative order

**Code Pattern**:
```cpp
int slow = 0;
for (int fast = 1; fast < n; fast++) {
    if (arr[fast] != arr[slow]) {
        slow++;
        arr[slow] = arr[fast];
    }
}
```

**Real-World Example**: Remove duplicates, remove element, move zeros

### Sliding Window (Two Pointers)

**Use When**:
- Variable size window
- Need to find optimal window
- Expand/shrink based on condition

**Code Pattern**:
```cpp
int left = 0;
for (int right = 0; right < n; right++) {
    // Expand window
    add_to_window(arr[right]);
    
    // Shrink until condition met
    while (condition_violated()) {
        remove_from_window(arr[left]);
        left++;
    }
    
    // Process window
}
```

**Real-World Example**: Longest substring, minimum window substring

### Merge Pattern

**Use When**:
- Two sorted sequences
- Need to merge or find intersection
- Need to compare elements

**Code Pattern**:
```cpp
int i = 0, j = 0;
while (i < n && j < m) {
    if (arr1[i] < arr2[j]) {
        i++;
    } else if (arr1[i] > arr2[j]) {
        j++;
    } else {
        // Found intersection
        i++; j++;
    }
}
```

**Real-World Example**: Merge sorted arrays, intersection of sorted arrays

## Pattern Recognition Checklist

### Before Implementing Two Pointers, Ask:

1. **Is data sorted?**
   - YES → Opposite ends or same direction
   - NO → Fast/slow or sliding window

2. **What am I looking for?**
   - Pairs/triplets → Opposite ends
   - Cycle/middle → Fast/slow
   - Substring/subarray → Sliding window
   - Merge/intersection → Merge pattern

3. **Can I eliminate search space?**
   - YES → Opposite ends (sorted array)
   - NO → May need different approach

4. **Do I need in-place modification?**
   - YES → Same direction (remove duplicates)
   - NO → Can use extra space

5. **Is it a linked list?**
   - YES → Fast/slow pointers
   - NO → Array-based two pointers

## Common Mistakes to Avoid

1. **Not handling empty/null cases**
   - Check for empty array/list
   - Check for null pointers

2. **Off-by-one errors**
   - Check boundary conditions
   - Verify loop termination

3. **Not updating both pointers**
   - Make sure both pointers move
   - Don't forget to update in all branches

4. **Wrong pointer movement**
   - Understand when to move which pointer
   - Don't move pointers incorrectly

5. **Not considering edge cases**
   - Single element
   - All elements same
   - No valid solution

## Real-World Recognition Examples

### Example 1: Linked List Cycle Detection

**Problem**: Detect if linked list has cycle

**Pattern Recognition**:
- Linked list structure
- Need to detect cycle without extra space
- Fast/slow pointers

**Variant**: Floyd's cycle detection

### Example 2: Container With Most Water

**Problem**: Find two lines that form container with most water

**Pattern Recognition**:
- Array of heights
- Need to find pair maximizing area
- Can eliminate search space

**Variant**: Opposite ends pattern

### Example 3: Remove Duplicates

**Problem**: Remove duplicates from sorted array in-place

**Pattern Recognition**:
- Sorted array
- In-place modification
- Two pointers same direction

**Variant**: Same direction (slow/fast)

### Example 4: Longest Substring Without Repeating Characters

**Problem**: Find longest substring with unique characters

**Pattern Recognition**:
- String/array
- Variable size window
- Expand/shrink based on condition

**Variant**: Sliding window (two pointers)

## Decision Tree

```
Need to process array/list?
├─ Sorted?
│  ├─ YES
│  │  ├─ Need pairs/triplets? → Opposite ends
│  │  ├─ Remove duplicates? → Same direction
│  │  └─ Merge/intersection? → Merge pattern
│  └─ NO
│     ├─ Linked list?
│     │  ├─ Cycle detection? → Fast/slow
│     │  ├─ Find middle? → Fast/slow
│     │  └─ Reverse/partition? → Two pointers
│     └─ Array/string?
│        ├─ Variable window? → Sliding window
│        └─ In-place modification? → Same direction
└─ Two sequences?
   └─ Merge/intersection? → Merge pattern
```

