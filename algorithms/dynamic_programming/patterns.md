# Dynamic Programming Problem Patterns

## 1. Knapsack Patterns
### 0/1 Knapsack
- Each item can be used exactly once
- Need to make a choice for each item: take it or leave it
- Examples:
  - Subset Sum
  - Partition Equal Subset Sum
  - Target Sum
  - Count of Subset Sum

### Unbounded Knapsack
- Each item can be used multiple times
- Examples:
  - Coin Change
  - Rod Cutting
  - Minimum Coin Change
  - Maximum Ribbon Cut

## 2. String Patterns
### String Matching/Editing
- Usually involves two strings
- Compare or manipulate characters
- Examples:
  - Longest Common Subsequence
  - Edit Distance
  - Minimum Deletions to Make String Palindrome
  - Shortest Common Supersequence

### Palindromic Subsequence
- Single string operations focusing on palindrome properties
- Examples:
  - Longest Palindromic Subsequence
  - Palindromic Substrings
  - Count of Palindromic Substrings

## 3. Matrix/Grid Patterns
### Path Finding
- Usually involves finding paths in a 2D grid
- Often includes obstacles or constraints
- Examples:
  - Minimum/Maximum Path Sum
  - Unique Paths I & II
  - Dungeon Game

### Matrix Chain Multiplication
- Optimal way to perform a series of operations
- Examples:
  - Matrix Chain Multiplication
  - Burst Balloons
  - Minimum Cost Tree From Leaf Values

## 4. Decision Making
### Buy/Sell Stock Patterns
- Series of decisions about when to buy/sell
- State transitions based on previous decisions
- Examples:
  - Best Time to Buy and Sell Stock (all variations)
  - Stock Trading with Transaction Fee
  - Stock Trading with Cooldown

### Game Theory
- Optimal play between two players
- Usually involves taking turns
- Examples:
  - Stone Game variations
  - Predict the Winner
  - Can I Win

## 5. Subsequence Patterns
### Increasing Subsequence
- Finding optimal subsequences with specific properties
- Examples:
  - Longest Increasing Subsequence
  - Number of Longest Increasing Subsequence
  - Russian Doll Envelopes

### Substring vs Subsequence
- Contiguous vs non-contiguous elements
- Examples:
  - Maximum Subarray
  - Longest Common Substring
  - Maximum Product Subarray

## 6. Partition Problems
### Array Partition
- Dividing array into subsets with specific properties
- Examples:
  - Partition Array for Maximum Sum
  - Palindrome Partitioning
  - Perfect Squares

### Interval Problems
- Usually involves merging or splitting intervals
- Examples:
  - Burst Balloons
  - Minimum Cost to Merge Stones
  - Remove Boxes

## How to Identify the Pattern

1. Look for keywords:
   - "Maximum/Minimum"
   - "Longest/Shortest"
   - "Count ways"
   - "Possibility"
   - "Optimization"

2. Analyze the input:
   - Single array/string → Likely subsequence or partition
   - Two strings → String matching pattern
   - Grid → Matrix pattern
   - Items with weights/values → Knapsack pattern
   - Sequence of decisions → Decision making pattern

3. Check constraints:
   - Can items be reused? → Unbounded vs 0/1 Knapsack
   - Need contiguous elements? → Substring vs Subsequence
   - Need to maintain order? → LIS vs general subsequence

4. State transition hints:
   - Take/not take decisions → Likely Knapsack
   - Match/don't match → String patterns
   - Different paths possible → Grid patterns
   - Buy/sell/skip → Decision making
