# Recursion Optimization Standards

## Overview
Recursion optimization is critical for production grade recursive algorithms. This document defines standards for optimizing recursive algorithms including memoization, dynamic programming, and iteration conversion.

## Memoization Optimization

### When to Use
* **Overlapping subproblems**: When subproblems overlap
* **Redundant computations**: When computations are redundant
* **Performance improvement**: When significant improvement expected
* **Rationale**: Memoization improves performance

### Implementation Patterns
* **Array cache**: Use array for cache
* **Hash table cache**: Use hash table for sparse problems
* **Check before compute**: Check cache before computing
* **Rationale**: Patterns enable efficient memoization

### Example Memoization
```c
// Memoized recursive function
int fibonacci_memo(int n, int* memo) {
    if (n <= 1) {
        return n;
    }
    if (memo[n] != -1) {
        return memo[n];  // Return cached
    }
    memo[n] = fibonacci_memo(n - 1, memo) + fibonacci_memo(n - 2, memo);
    return memo[n];
}
```

## Dynamic Programming

### Bottom Up Approach
* **Definition**: Build solution from base cases
* **Benefits**: Eliminates recursion overhead
* **Use cases**: When memoization is applicable
* **Rationale**: Bottom up approach improves performance

### Top Down Approach
* **Definition**: Memoized recursion
* **Benefits**: Natural recursive structure
* **Use cases**: When recursion is natural
* **Rationale**: Top down approach maintains structure

### Example Dynamic Programming
```c
// Bottom up Fibonacci
int fibonacci_dp(int n) {
    if (n <= 1) return n;
    int* dp = (int*)malloc((n + 1) * sizeof(int));
    dp[0] = 0;
    dp[1] = 1;
    for (int i = 2; i <= n; i++) {
        dp[i] = dp[i - 1] + dp[i - 2];
    }
    int result = dp[n];
    free(dp);
    return result;
}
```

## Tail Recursion Optimization

### Tail Call Optimization
* **Definition**: Compiler optimization for tail recursion
* **Benefits**: Eliminates stack growth
* **Limitations**: Not always applicable
* **Rationale**: Optimization improves performance

### Manual Optimization
* **Conversion**: Convert to iteration manually
* **Benefits**: Guaranteed optimization
* **Use cases**: When compiler optimization uncertain
* **Rationale**: Manual optimization ensures performance

## Iteration Conversion

### When to Convert
* **Stack overflow**: When stack overflow is concern
* **Performance**: When iteration is more efficient
* **Clarity**: When iteration is clearer
* **Rationale**: Conversion prevents issues

### How to Convert
* **Explicit stack**: Use explicit stack data structure
* **State machine**: Use state machine
* **Loop**: Use loop with state
* **Rationale**: Conversion enables iteration

## Performance Benchmarking

### Metrics
* **Time complexity**: Measure time complexity
* **Space complexity**: Measure space complexity
* **Stack usage**: Measure stack usage
* **Rationale**: Metrics enable evaluation

### Benchmarking Tools
* **Google Benchmark**: C++ benchmarking framework
* **perf**: Linux performance profiling
* **Rationale**: Tools enable performance analysis

## Implementation Standards

### Performance Targets
* **Time complexity**: Optimize time complexity
* **Space complexity**: Optimize space complexity
* **Stack usage**: Minimize stack usage
* **Rationale**: Targets ensure good performance

### Optimization
* **Profile first**: Profile before optimizing
* **Identify bottlenecks**: Identify performance bottlenecks
* **Optimize hot paths**: Optimize frequently executed code
* **Rationale**: Data driven optimization

## Testing Requirements

### Performance Tests
* **Benchmarks**: Benchmark critical operations
* **Comparison**: Compare optimized vs unoptimized
* **Stack usage**: Measure stack usage
* **Rationale**: Performance tests ensure goals

## Research Papers and References

### Optimization
* "Introduction to Algorithms" (CLRS) - Optimization techniques
* "Dynamic Programming" research papers
* Optimization guides

## Implementation Checklist

- [ ] Identify optimization opportunities
- [ ] Implement memoization when applicable
- [ ] Consider dynamic programming
- [ ] Optimize tail recursion
- [ ] Convert to iteration when needed
- [ ] Benchmark performance
- [ ] Profile and optimize bottlenecks
- [ ] Document optimization decisions

