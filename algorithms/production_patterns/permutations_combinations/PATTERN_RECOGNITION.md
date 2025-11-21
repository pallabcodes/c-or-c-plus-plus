# Permutations and Combinations Pattern Recognition

## When to Recognize Permutation/Combination Opportunity

### Input Characteristics That Suggest Permutation/Combination

1. **Subset Generation Problems**
   - Generate all possible arrangements of elements
   - Find all combinations of k elements from n
   - Subset sum problems
   - Power set generation

2. **Arrangement Problems**
   - All possible ways to arrange items
   - Sequence generation with constraints
   - Word permutations
   - Path finding in graphs

3. **Selection Problems**
   - Choose k items from n without regard to order
   - Lottery number generation
   - Team selection combinations
   - Feature subset selection

4. **Optimization Problems**
   - Traveling salesman (permutations of cities)
   - Assignment problems
   - Resource allocation
   - Scheduling permutations

5. **Cryptography and Security**
   - Password generation
   - Key permutation testing
   - Brute force attack simulations
   - Cryptographic key spaces

## Variant Selection Guide

### Decision Tree

```
Need permutations/combinations?
│
├─ Need all arrangements (order matters)?
│  └─ YES → Permutation variants
│
├─ Need selections (order doesn't matter)?
│  └─ YES → Combination variants
│
├─ Large n/small k (combinations)?
│  └─ YES → Lexicographic combinations
│
├─ Need next permutation only?
│  └─ YES → STL-style next_permutation
│
├─ Memory constrained?
│  └─ YES → Iterator-based generators
│
├─ Need all permutations efficiently?
│  └─ YES → Heap's algorithm or Steinhaus-Johnson-Trotter
│
├─ Need combinatorial indexing?
│  └─ YES → Combinatorial number systems
│
└─ Need subset generation?
   └─ YES → Bit manipulation or recursive backtracking
```

### Variant Comparison

| Variant | Best For | Key Feature | Time Complexity | Space Complexity |
|---------|----------|-------------|-----------------|------------------|
| Next Permutation | Single next arrangement | Lexicographic successor | O(n) | O(1) |
| All Permutations | Complete enumeration | Backtracking generation | O(n!) | O(n) |
| Heap's Algorithm | In-place permutations | Non-recursive | O(n!) | O(1) |
| Lex Combinations | k-combinations | Dictionary order | O(C(n,k)) | O(k) |
| Backtracking Comb | Flexible constraints | Custom filters | O(2^n) worst | O(depth) |
| Bit Manipulation | Subset generation | Bit masks | O(2^n) | O(n) |
| Combinatorial Numbers | Ranking/unranking | Bijection | O(k) | O(k) |

## Detailed Variant Selection

### 1. Next Permutation (STL-Style)

**When to Use:**
- Generate permutations in lexicographic order
- Need only the next permutation, not all
- Memory-constrained applications
- Standard library replacement
- Dictionary order enumeration

**Key Characteristics:**
- Finds next permutation in lexicographic order
- Returns false when no more permutations exist
- In-place modification
- O(n) time complexity
- Used in C++ STL algorithms

**Real-World Examples:**
- C++ `std::next_permutation`
- Dictionary word generation
- Sorting algorithm variants
- Competitive programming problems

### 2. All Permutations (Backtracking)

**When to Use:**
- Need all possible arrangements
- Small input sizes (n ≤ 10-12)
- Custom constraints on permutations
- Educational implementations
- When you need to process each permutation

**Key Characteristics:**
- Recursive backtracking approach
- Generates all n! permutations
- Easy to add constraints
- Stack overflow risk for large n
- Good for learning algorithms

**Real-World Examples:**
- Word permutation generators
- Puzzle solvers
- Combinatorial optimization
- Brute force algorithms

### 3. Heap's Algorithm

**When to Use:**
- Generate all permutations efficiently
- In-place permutation generation
- No recursion (stack-safe)
- Competitive programming
- When you need all permutations

**Key Characteristics:**
- Non-recursive algorithm
- Generates each permutation exactly once
- O(n) space for swaps
- Time complexity O(n!)
- Based on research by B.R. Heap

**Real-World Examples:**
- Competitive programming libraries
- Algorithm research implementations
- Educational algorithm collections
- Production permutation generators

### 4. Combination Generators

**When to Use:**
- Need subsets of size k from n elements
- Order doesn't matter
- Lottery number generation
- Team selection
- Feature subset selection

**Key Characteristics:**
- Generates C(n,k) combinations
- Lexicographic order
- Memory efficient
- Recursive or iterative implementations
- Used in combinatorial algorithms

**Real-World Examples:**
- Lottery number generators
- Subset selection algorithms
- Combinatorial optimization
- Statistical sampling

### 5. Combinatorial Number Systems

**When to Use:**
- Need to rank/unrank combinations
- Bijection between combinations and integers
- Efficient combination storage/lookup
- Combinatorial algorithms
- Mathematical computing

**Key Characteristics:**
- Maps combinations to unique integers
- O(k) time complexity
- Memory efficient representation
- Used in combinatorial mathematics
- Enables efficient storage

**Real-World Examples:**
- Mathematical computing libraries
- Combinatorial algorithm research
- Database indexing schemes
- Cryptographic applications

### 6. Bit Manipulation Subsets

**When to Use:**
- Generate all 2^n subsets
- Small n (n ≤ 20-25)
- Bit-level operations needed
- Memory-efficient subset representation
- Hardware-accelerated operations

**Key Characteristics:**
- Uses bit masks for subsets
- O(2^n) time for all subsets
- O(1) space per subset
- Fast set operations
- Hardware optimization possible

**Real-World Examples:**
- Dynamic programming (subset sum)
- Bit-parallel algorithms
- Hardware-accelerated computing
- Embedded systems

## Performance Characteristics

### Complexity Analysis

| Operation | Permutations | Combinations | Subsets |
|-----------|--------------|--------------|---------|
| Time | O(n!) | O(C(n,k)) | O(2^n) |
| Space | O(n) | O(k) | O(1) per subset |
| Practical n limit | 10-12 | 20-30 | 20-25 |

### Memory Usage Patterns

| Algorithm | Memory Pattern | Best For |
|-----------|----------------|----------|
| Next Permutation | In-place | Single operations |
| Backtracking | Stack-based | Small inputs |
| Heap's Algorithm | Minimal | All permutations |
| Lex Combinations | Linear in k | Large n, small k |
| Bit Subsets | Constant | Small n |

## Use Case Mapping

### Competitive Programming
- **Best Choice**: Heap's algorithm for permutations
- **Reason**: Efficient, non-recursive, all permutations
- **Alternatives**: Backtracking for constrained permutations

### Data Analysis
- **Best Choice**: Combination generators
- **Reason**: Efficient subset selection
- **Alternatives**: Bit manipulation for small sets

### Game Development
- **Best Choice**: Next permutation
- **Reason**: Level generation, sequence variation
- **Alternatives**: Lex combinations for team selection

### Scientific Computing
- **Best Choice**: Combinatorial number systems
- **Reason**: Efficient indexing and storage
- **Alternatives**: Lex combinations for sampling

### Embedded Systems
- **Best Choice**: Bit manipulation subsets
- **Reason**: Minimal memory usage
- **Alternatives**: Next permutation for sequences

## Key Patterns Extracted

### Pattern 1: Lexicographic Ordering
- **Found in**: STL algorithms, dictionary generation
- **Technique**: Generate in sorted order
- **Benefit**: Predictable enumeration order
- **Trade-off**: Not the most efficient for all cases

### Pattern 2: Backtracking Generation
- **Found in**: Constraint programming, puzzle solvers
- **Technique**: Recursive exploration with pruning
- **Benefit**: Easy to add constraints
- **Trade-off**: Stack usage and performance

### Pattern 3: In-Place Modification
- **Found in**: Memory-constrained applications
- **Technique**: Modify input array directly
- **Benefit**: Minimal memory overhead
- **Trade-off**: Original data is modified

### Pattern 4: Iterator-Based Access
- **Found in**: Modern C++ ranges, lazy evaluation
- **Technique**: Generate on-demand
- **Benefit**: Memory efficient for large sets
- **Trade-off**: Random access limitations

### Pattern 5: Bit-Level Operations
- **Found in**: Hardware-accelerated algorithms
- **Technique**: Use integer bits as set representation
- **Benefit**: Fast operations, compact storage
- **Trade-off**: Limited to small sets

## Real-World Examples

### C++ STL next_permutation
- **Pattern**: Lexicographic successor generation
- **Usage**: Dictionary order permutation enumeration
- **Why**: Standard library implementation, efficient

### Heap's Algorithm Implementation
- **Pattern**: Non-recursive permutation generation
- **Usage**: Competitive programming libraries
- **Why**: Stack-safe, generates all permutations

### Lottery Number Generators
- **Pattern**: Combination generation
- **Usage**: Random number selection without replacement
- **Why**: Efficient for large pools, small selections

### Password Generators
- **Pattern**: Permutation with constraints
- **Usage**: Secure password generation
- **Why**: Customizable character constraints

### Subset Sum Algorithms
- **Pattern**: Bit manipulation subsets
- **Usage**: Dynamic programming optimization
- **Why**: Fast set operations, memory efficient

### Traveling Salesman Problem
- **Pattern**: Permutation generation
- **Usage**: Brute force solution enumeration
- **Why**: Complete search space exploration

## References

### Production Codebases
- C++ STL: `<algorithm>` header (std::next_permutation)
- Boost Combinatorics: Permutation and combination libraries
- Python itertools: Iterator-based generators

### Research Papers
- "Permutation Generation Methods" - Robert Sedgewick
- "Efficient Generation of Combinatorial Objects" - various papers
- "Heap's Algorithm for Permutation Generation" - B.R. Heap

### Books and Textbooks
- "The Art of Computer Programming" Vol 4A - Donald Knuth
- "Combinatorial Algorithms" - Albert Nijenhuis and Herbert Wilf
- "Algorithms" - Robert Sedgewick

### Online Resources
- Competitive programming tutorial sites
- Algorithm visualization platforms
- Mathematics computation libraries

### Technical Blogs
- Algorithm implementation blogs
- Competitive programming blogs
- Mathematics computing articles
