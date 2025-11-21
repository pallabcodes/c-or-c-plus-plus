# Sliding Window Pattern Recognition

## When to Recognize Sliding Window Opportunity

### Input Characteristics That Suggest Sliding Window

1. **Subarray/Substring Problems**
   - "Find maximum/minimum in subarray of size k"
   - "Find longest substring with property X"
   - "Find all subarrays/substrings matching pattern"

2. **Consecutive Elements**
   - Need to process consecutive elements
   - Window size is fixed or variable
   - Moving average, sum, or aggregate

3. **Time-Based Windows**
   - "Events in last N seconds"
   - "Rate limiting per time window"
   - "Moving statistics over time"

4. **Optimization Opportunity**
   - Brute force would be O(n*k) or O(n²)
   - Can reuse computation from previous window
   - Overlapping windows

### Problem Patterns That Use Sliding Window

1. **Fixed Size Window**
   - Maximum sum of subarray of size k
   - Average of subarrays of size k
   - Maximum/minimum in sliding window

2. **Variable Size Window**
   - Longest substring with K distinct characters
   - Minimum window substring
   - Longest substring without repeating characters

3. **Time-Based Window**
   - Rate limiting (requests per time window)
   - Moving averages over time
   - Event counting in time window

4. **Two Pointers Variant**
   - Expand window until condition met
   - Shrink window until condition violated
   - Track window state (frequency map, sum, etc.)

## Variant Selection Guide

### Fixed Size Window (Simple)

**Use When**:
- Window size is constant
- Need aggregate (sum, max, min) over window
- Simple sliding operation

**Code Pattern**:
```cpp
// Initialize window
int window_sum = 0;
for (int i = 0; i < k; i++) {
    window_sum += arr[i];
}

// Slide window
for (int i = k; i < n; i++) {
    window_sum = window_sum - arr[i-k] + arr[i];
    // Process window_sum
}
```

**Real-World Example**: Maximum sum subarray of size k

### Variable Size Window (Expand/Shrink)

**Use When**:
- Window size varies based on condition
- Need to find optimal window
- Condition-based expansion/shrinking

**Code Pattern**:
```cpp
int left = 0;
for (int right = 0; right < n; right++) {
    // Expand window
    add_to_window(arr[right]);
    
    // Shrink window until condition met
    while (condition_violated()) {
        remove_from_window(arr[left]);
        left++;
    }
    
    // Process valid window
    process_window(left, right);
}
```

**Real-World Example**: Longest substring with K distinct characters

### Deque-Based Maximum/Minimum Window

**Use When**:
- Need maximum/minimum in sliding window
- Need O(1) access to window max/min
- Window size is fixed

**Code Pattern**:
```cpp
deque<int> dq; // Stores indices
for (int i = 0; i < n; i++) {
    // Remove indices outside window
    while (!dq.empty() && dq.front() <= i - k) {
        dq.pop_front();
    }
    
    // Remove smaller elements (for max window)
    while (!dq.empty() && arr[dq.back()] <= arr[i]) {
        dq.pop_back();
    }
    
    dq.push_back(i);
    if (i >= k - 1) {
        // Window max is arr[dq.front()]
    }
}
```

**Real-World Example**: Sliding window maximum

### Time-Based Window (Ring Buffer)

**Use When**:
- Events have timestamps
- Need to maintain window over time
- Old events expire automatically

**Code Pattern**:
```cpp
deque<pair<int, T>> window; // (timestamp, value)
int window_size; // Time window size

void add_event(int timestamp, T value) {
    // Remove expired events
    while (!window.empty() && 
           window.front().first <= timestamp - window_size) {
        window.pop_front();
    }
    
    window.push_back({timestamp, value});
}
```

**Real-World Example**: Rate limiting, moving averages

### Frequency Map Window

**Use When**:
- Need to track character/element frequencies
- Window contains distinct elements
- Need to check frequency conditions

**Code Pattern**:
```cpp
unordered_map<char, int> freq;
int distinct_count = 0;

void add_to_window(char c) {
    if (freq[c] == 0) distinct_count++;
    freq[c]++;
}

void remove_from_window(char c) {
    freq[c]--;
    if (freq[c] == 0) distinct_count--;
}
```

**Real-World Example**: Anagrams, distinct character substrings

## Pattern Recognition Checklist

### Before Implementing Sliding Window, Ask:

1. **What am I processing?**
   - Subarrays → Fixed/variable window
   - Substrings → Variable window with frequency map
   - Time-based events → Time-based window

2. **Is window size fixed or variable?**
   - Fixed → Simple sliding or deque-based
   - Variable → Expand/shrink pattern

3. **What do I need to track?**
   - Sum → Simple sliding
   - Max/Min → Deque-based
   - Frequencies → Frequency map
   - Timestamps → Time-based window

4. **Can I reuse computation?**
   - YES → Sliding window (reuse previous window)
   - NO → May need different approach

5. **What's the constraint?**
   - Size constraint → Fixed window
   - Property constraint → Variable window
   - Time constraint → Time-based window

## Common Mistakes to Avoid

1. **Not removing old elements**
   - Always remove elements leaving window
   - Check window boundaries

2. **Not handling empty window**
   - Check if window is empty before processing
   - Handle edge cases (array size < window size)

3. **Inefficient window updates**
   - Reuse computation from previous window
   - Don't recompute entire window each time

4. **Wrong data structure**
   - Use deque for max/min windows
   - Use frequency map for distinct element tracking
   - Use simple sliding for sums

5. **Not considering time-based windows**
   - Time-based problems need timestamp tracking
   - Don't confuse size-based with time-based

## Real-World Recognition Examples

### Example 1: Rate Limiting

**Problem**: Limit requests to N per time window

**Pattern Recognition**:
- Time-based window
- Need to track timestamps
- Remove expired requests

**Variant**: Time-based ring buffer

### Example 2: Network Packet Analysis

**Problem**: Analyze packets in sliding time window

**Pattern Recognition**:
- Time-based window
- Need to process recent packets
- Remove old packets

**Variant**: Time-based deque

### Example 3: Stock Price Analysis

**Problem**: Moving average of stock prices

**Pattern Recognition**:
- Fixed size window (N days)
- Need sum/average
- Simple sliding window

**Variant**: Fixed size sliding window

### Example 4: Log Analysis

**Problem**: Find patterns in log entries within time window

**Pattern Recognition**:
- Time-based window
- Need frequency tracking
- Variable window based on pattern

**Variant**: Time-based window with frequency map

## Decision Tree

```
Need to process subarray/substring?
├─ Fixed size?
│  ├─ Need max/min? → Deque-based window
│  └─ Need sum/avg? → Simple sliding window
├─ Variable size?
│  ├─ Based on distinct elements? → Frequency map window
│  ├─ Based on condition? → Expand/shrink window
│  └─ Based on pattern? → Frequency map + expand/shrink
└─ Time-based?
   ├─ Fixed time window? → Time-based ring buffer
   └─ Variable time window? → Time-based deque
```

