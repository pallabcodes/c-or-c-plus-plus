# Recursion Algorithm Extraction Notes

## Summary

Extracted 3 recursion algorithm variants from production codebases:
- **LLVM Recursive Descent Parser**: Top-down parsing with operator precedence
- **Tree Recursion**: Tree traversal, divide and conquer, memoization
- **Tail Recursion**: Tail call optimization, iterative conversion

## Extracted Variants

### 1. LLVM Recursive Descent Parser

**Source**: https://github.com/llvm/llvm-project
**Repository**: llvm/llvm-project
**File**: `clang/lib/Parse/ParseExpr.cpp`
**Variant File**: `production_patterns/recursion/variants/llvm_recursive_descent.cpp`

**Key Features**:
- Recursive descent: Each grammar rule = function
- Operator precedence: Handles precedence correctly
- Error recovery: Continues parsing after errors
- Top-down parsing: Natural for LL grammars

**Key Insights**:
- Recursive descent is natural for LL grammars
- Operator precedence parsing handles expressions correctly
- Error recovery improves user experience
- Used in production compilers (Clang/LLVM)

**Performance Characteristics**:
- Parsing: O(n) where n is number of tokens
- Space: O(d) where d is maximum recursion depth

**Use Cases**:
- LL(1) or LL(k) grammars
- Expression parsing
- Language parsers
- Compiler frontends

### 2. Tree Recursion

**Source**: Various production codebases and algorithms
**Variant File**: `production_patterns/recursion/variants/tree_recursion.cpp`

**Key Features**:
- Multiple recursive calls: Creates tree structure
- Divide and conquer: Break problem into subproblems
- Memoization: Cache results to avoid recomputation
- Tree traversal: Pre-order, in-order, post-order

**Key Insights**:
- Tree recursion natural for tree data structures
- Memoization eliminates redundant computation
- Divide and conquer enables efficient algorithms
- Used extensively in compilers and data structures

**Performance Characteristics**:
- Without memoization: O(2^n) for binary tree recursion
- With memoization: O(n) for n subproblems
- Tree traversal: O(n) where n is number of nodes
- Space: O(h) where h is height of recursion tree

**Use Cases**:
- Tree data structures
- Divide and conquer problems
- Problems with overlapping subproblems
- Tree traversal

### 3. Tail Recursion

**Source**: Compiler optimization techniques
**Variant File**: `production_patterns/recursion/variants/tail_recursion.cpp`

**Key Features**:
- Tail recursion: Recursive call is last operation
- Tail call elimination: Compiler converts to iteration
- Stack optimization: O(1) space instead of O(n)
- Performance: Same as iteration, more readable

**Key Insights**:
- Tail recursion enables stack optimization
- Compiler can convert to iteration automatically
- Same performance as iteration, more readable
- Used in functional languages and compilers

**Performance Characteristics**:
- Time: Same as iterative version
- Space: O(1) with optimization, O(n) without

**Use Cases**:
- Recursive algorithms where last operation is recursive call
- Stack space limited
- Functional programming style
- When compiler supports tail call optimization

## Comparison of Variants

### Performance Comparison

| Variant | Time Complexity | Space Complexity | Best For |
|---------|----------------|------------------|----------|
| Recursive Descent | O(n) | O(d) | Parsing |
| Tree Recursion | O(2^n) without memo | O(h) | Tree problems |
| Tail Recursion | Same as iteration | O(1) optimized | Iterative problems |

### When to Use Each Variant

**Recursive Descent Parser**:
- LL(1) or LL(k) grammars
- Expression parsing
- Language parsers
- Compiler frontends

**Tree Recursion**:
- Tree data structures
- Divide and conquer
- Problems with overlapping subproblems
- Tree traversal

**Tail Recursion**:
- Last operation is recursive call
- Stack space limited
- Functional programming style
- When compiler supports optimization

## Key Patterns Extracted

### Pattern 1: Recursive Descent
- **Found in**: LLVM parser, GCC parser
- **Technique**: Each grammar rule = function
- **Benefit**: Natural, easy to understand
- **Trade-off**: Limited to LL grammars

### Pattern 2: Memoization
- **Found in**: Tree recursion
- **Technique**: Cache recursive results
- **Benefit**: Eliminates redundant computation
- **Trade-off**: Memory overhead

### Pattern 3: Tail Call Optimization
- **Found in**: Tail recursion
- **Technique**: Convert tail calls to iteration
- **Benefit**: O(1) space complexity
- **Trade-off**: Requires compiler support

### Pattern 4: Divide and Conquer
- **Found in**: Tree recursion
- **Technique**: Split problem into subproblems
- **Benefit**: Natural problem decomposition
- **Trade-off**: Overhead for splitting

## Source Attribution

### LLVM
- **Repository**: https://github.com/llvm/llvm-project
- **License**: Apache 2.0 with LLVM Exceptions
- **Author**: LLVM team
- **Key Contributors**: Various LLVM developers

### Tree Recursion
- **Source**: Various production codebases
- **Patterns**: Standard tree recursion patterns
- **Applications**: Compilers, data structures, algorithms

### Tail Recursion
- **Source**: Compiler optimization techniques
- **Patterns**: Tail call elimination
- **Applications**: Functional languages, compilers

## Extraction Insights

### Common Optimizations

1. **Memoization**: Cache recursive results (tree recursion)
2. **Tail Call Optimization**: Convert to iteration (tail recursion)
3. **Early Termination**: Stop recursion early when possible
4. **Iterative Conversion**: Convert recursion to iteration when beneficial
5. **Stack Optimization**: Minimize stack usage

### Production-Grade Techniques

1. **Recursive Descent**: Natural for LL grammars
2. **Memoization**: Eliminates redundant computation
3. **Tail Call Optimization**: Stack space optimization
4. **Divide and Conquer**: Natural problem decomposition
5. **Error Recovery**: Continue after errors (parsing)

### Lessons Learned

1. **Recursive descent is natural for LL grammars** (LLVM parser)
2. **Memoization dramatically improves performance** (tree recursion)
3. **Tail recursion enables stack optimization** (tail recursion)
4. **Divide and conquer enables efficient algorithms** (tree recursion)
5. **Error recovery improves user experience** (parsing)

## References

- LLVM: https://github.com/llvm/llvm-project
- Recursive Descent Parsing: Various compiler textbooks
- Tree Recursion: Standard algorithm patterns
- Tail Recursion: Compiler optimization techniques

