# Advanced Bitwise Data Structures

## Scope
Applies to advanced data structures using bit manipulation including succinct bitvectors, rank/select operations, bitslicing, and compressed encodings.

## Succinct Bitvectors

### Rank Operations
* Rank1(i): Count number of 1s in positions [0, i)
* Rank0(i): Count number of 0s in positions [0, i)
* O(1) query time with O(n) space overhead
* Multi level indexing structure
* Used in compressed data structures

### Select Operations
* Select1(k): Find position of k th 1
* Select0(k): Find position of k th 0
* O(log n) query time typically
* More complex than rank operations
* Used with rank for advanced queries

### Implementation Structure
* Base bitvector storage
* Level 1: Coarse grained rank counts (every 512 bits)
* Level 2: Fine grained rank counts (every 64 bits)
* Popcount for final word
* Balance space vs query time

### Code Quality Standards
* Document space overhead
* Explain query time complexity
* Provide clear API documentation
* Handle edge cases (empty bitvector, k > count)
* Optimize for common access patterns

## Bitslicing

### Concept
* Store multiple boolean values in single register
* Each bit position represents different value
* Enable parallel boolean operations via SIMD
* Used in cryptography, database systems, signal processing

### Implementation
* Pack boolean values into bit positions
* Use SIMD instructions for parallel operations
* Extract individual results when needed
* Consider endianness effects
* Optimize for bulk operations

### Applications
* Parallel boolean evaluation
* Database bitmap operations
* Cryptographic operations
* Signal processing filters

## Compressed Encodings

### Elias-Fano Encoding
* Compressed representation of monotone sequences
* Balance compression ratio vs query speed
* Used in inverted indexes
* Supports rank and select operations

### Golomb-Rice Encoding
* Compressed representation of integers
* Parameterized by divisor
* Used in compression algorithms
* Efficient encoding and decoding

### Implementation Considerations
* Trade off compression vs speed
* Consider access patterns
* Optimize for common queries
* Document compression characteristics

## Roaring Bitmaps

### Structure
* Container based organization
* Array container: Sparse sets
* Bitmap container: Dense sets
* Run container: Compressed runs
* Efficient set operations

### Operations
* Union, intersection, difference
* Population count
* Membership testing
* Range queries
* Used in production systems (Apache Druid, etc.)

### Implementation
* Consider using existing library
* Document space and time complexity
* Optimize for common operations
* Handle edge cases appropriately

## Code Quality Standards

### Documentation
* Explain data structure properties
* Document space and time complexity
* Reference research papers
* Provide usage examples
* Note implementation trade offs

### API Design
* Clear function names
* Consistent parameter ordering
* Appropriate return types
* Handle errors appropriately
* Document thread safety

### Performance
* Profile critical operations
* Optimize for common use cases
* Consider cache effects
* Document performance characteristics
* Benchmark against alternatives

### Error Handling
* Validate inputs
* Handle edge cases (empty structures, out of range)
* Provide meaningful error messages
* Consider failure modes
* Document undefined behavior

## Testing Requirements
* Test with various data distributions
* Verify correctness against reference implementation
* Test edge cases (empty, single element, all ones)
* Performance testing with realistic workloads
* Stress testing for large inputs
* Verify space usage matches expectations

## Research References
* Succinct Data Structures: "Succinct Data Structures" (Jacobson, 1989)
* Rank/Select: Various papers on efficient implementations
* Roaring Bitmaps: "Roaring Bitmaps" (Chambi et al., 2016)
* Bitslicing: Used in various cryptographic implementations

## Related Topics
* Advanced Techniques: Bit manipulation techniques used
* Performance Optimization: SIMD for bitslicing
* Enterprise Patterns: Production usage of these structures

