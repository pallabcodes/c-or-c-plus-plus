Yes, these 6 core patterns cover about 90-95% of DP problems you'll encounter in FAANG/product-based company interviews. However, let me highlight a few important points:

Pattern Combinations


Some interview problems combine 2-3 patterns together
For example, "Russian Doll Envelopes" combines LIS (Subsequence) with 2D sorting
"Maximum Height by Stacking Cuboids" combines 3D sorting with LIS


Interview Focus Areas


FAANG particularly loves:

String problems (especially LCS and Edit Distance variations)
Matrix path problems (especially unique paths and minimum path sum)
Buy/Sell Stock variations (Meta/Facebook is known for these)
LIS variations (Google often asks these)




What's Not Covered But Sometimes Asked


State Compression DP (rare but appears in Google)
Digit DP (very rare, mostly in Amazon)
Probability DP (rare, sometimes in Google/Meta)
Tree DP (usually masked as tree problems rather than pure DP)


Interview Strategy Tips


Focus deeply on understanding 0/1 Knapsack and LIS patterns first
They're the most fundamental and other patterns often build on their concepts
Practice converting word problems into these patterns
Many interviewers modify known problems slightly to test pattern recognition


Common Interview Twists


Adding constraints (like time limits or move restrictions)
Asking for space optimization (1D array instead of 2D)
Requesting print actual solution/path not just value
Combining with other data structures (like heap or binary search)

Would you like me to provide specific examples of how these patterns are modified in actual FAANG interviews? Or would you prefer to practice mapping some actual interview problems to these patterns?


--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

# Common DP Pattern Variations in FAANG Interviews

## 1. Knapsack Variations

### Basic: 0/1 Knapsack
- Given weights and values, maximize value within weight limit

### Interview Twists:
1. **Google**: Paint House III (LC 1473)
   - Basic pattern: Cost minimization (like knapsack)
   - Twist: Added neighborhood constraint
   - Extra dimension in DP state
   ```python
   dp[i][j][k]  # i: house, j: color, k: neighborhoods formed
   ```

2. **Amazon**: Shopping Offers (LC 638)
   - Basic pattern: 0/1 Knapsack
   - Twist: Items can be bought individually or in special offers
   - Need to handle both individual and package decisions

## 2. String Pattern Variations

### Basic: Longest Common Subsequence
- Find longest sequence present in both strings

### Interview Twists:
1. **Facebook**: Regular Expression Matching (LC 10)
   - Basic pattern: String matching
   - Twist: Added wildcard and pattern matching
   - Extra handling for '*' and '.' characters

2. **Google**: Interleaving String (LC 97)
   - Basic pattern: String matching
   - Twist: Match with two source strings
   ```python
   dp[i][j]  # Can we form target[0:i+j] using s1[0:i] and s2[0:j]
   ```

## 3. Matrix/Grid Variations

### Basic: Minimum Path Sum
- Find path from top-left to bottom-right minimizing sum

### Interview Twists:
1. **Amazon**: Dungeon Game (LC 174)
   - Basic pattern: Grid traversal
   - Twist: Minimum initial health needed
   - Reverse DP direction (bottom-up becomes literally bottom-up)

2. **Google**: Cherry Pickup II (LC 1463)
   - Basic pattern: Grid paths
   - Twist: Two robots moving simultaneously
   ```python
   dp[row][col1][col2]  # Maximum cherries with robots at (row,col1) and (row,col2)
   ```

## 4. Decision Making Variations

### Basic: Buy/Sell Stock
- Find best times to buy and sell

### Interview Twists:
1. **Facebook**: Best Time to Buy and Sell Stock with Transaction Fee (LC 714)
   - Basic pattern: Buy/Sell decisions
   - Twist: Added transaction cost
   - State needs to track current holdings

2. **Google**: Stone Game III (LC 1406)
   - Basic pattern: Game theory
   - Twist: Can take up to 3 stones
   - Need to track relative score difference

## 5. Subsequence Variations

### Basic: Longest Increasing Subsequence
- Find longest strictly increasing subsequence

### Interview Twists:
1. **Amazon**: Maximum Height by Stacking Cuboids (LC 1691)
   - Basic pattern: LIS
   - Twist: 3D comparison and rotation allowed
   - Need to preprocess and sort dimensions

2. **Google**: Number of Longest Increasing Subsequences (LC 673)
   - Basic pattern: LIS
   - Twist: Count all possible LIS
   - Need extra DP array for count

## 6. Partition Variations

### Basic: Palindrome Partitioning
- Split string into palindromic substrings

### Interview Twists:
1. **Facebook**: Split Array Largest Sum (LC 410)
   - Basic pattern: Array partition
   - Twist: Minimize the largest subarray sum
   - Can be solved with DP or binary search

2. **Google**: Minimum Cost to Cut a Stick (LC 1547)
   - Basic pattern: Partition
   - Twist: Cost depends on current segment length
   ```python
   dp[i][j]  # Minimum cost to cut segment [i,j] with all cuts in between
   ```

## Key Interview Tips:
1. When you see a twist:
   - First identify the base pattern
   - Then understand what new dimension the twist adds
   - Add that dimension to your DP state

2. Space optimization is often expected:
   - 2D to 1D array optimization
   - Rolling array technique
   - State compression

3. Follow-up questions often involve:
   - Printing the actual solution path
   - Handling constraints like k operations
   - Real-time/streaming versions
