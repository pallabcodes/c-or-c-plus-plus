# Enterprise Struct Patterns

## Scope
Applies to production struct patterns from top tier companies including Google, Bloomberg, Uber, Amazon, and PayPal.

## Google Production Patterns

### Search Index Structures
* Inverted index posting entries
* Compact document identifiers
* Term frequency encoding
* Position lists with compression
* Cache line aligned for performance

### Machine Learning Structures
* Feature vectors aligned for SIMD
* Query feature blocks
* Ranking signal structures
* Hot/cold path separation
* Batch processing structures

### Implementation Patterns
* Align to cache line boundaries (64 bytes)
* Group hot path members together
* Use appropriate integer sizes
* Minimize padding through ordering
* Document alignment requirements

### Code Example
```cpp
// Thread-safety: Thread-safe for concurrent reads
// Ownership: Value semantics
// Invariants: doc_id > 0, tf > 0
// Failure modes: None
struct alignas(64) RankingSignals {
    // Hot path - frequently accessed
    float bm25;
    float pagerank;
    float freshness;
    float click_prior;
    // Padding to cache line boundary
    float padding[12];
    // Cold path - less frequently accessed
    uint32_t doc_length;
    uint32_t link_count;
};
```

## Bloomberg Market Data Patterns

### Financial Data Structures
* Market data with precise timestamps
* Price and volume encoding
* Order book structures
* Trade structures with flags
* Compact encoding for high frequency data

### Real Time Feed Structures
* Low latency data structures
* Lock free access patterns
* Memory mapped structures
* Zero copy data transfer
* Hardware timestamp support

### Implementation Patterns
* Fixed size structures for predictability
* Packed structures for network protocols
* Bit fields for flag encoding
* Timestamp precision considerations
* Endianness handling

## Uber Real Time Systems

### Ride Matching Structures
* Location data with spatial indexing
* Driver and rider structures
* Matching algorithm data structures
* Real time dispatch structures
* Concurrent access optimization

### Implementation Patterns
* Lock free structures for concurrent access
* Memory pool allocation
* Cache friendly layouts
* SIMD alignment for location calculations
* Zero copy for network communication

## Amazon Production Patterns

### E Commerce Structures
* Product catalog structures
* Shopping cart structures
* Order processing structures
* Inventory management structures
* Recommendation system structures

### Cloud Service Structures
* Service configuration structures
* Resource allocation structures
* Monitoring and metrics structures
* Distributed system structures
* Versioning and compatibility

### Implementation Patterns
* Versioned structures for compatibility
* Serialization friendly layouts
* Network protocol structures
* Database storage structures
* API request/response structures

## PayPal Payment Systems

### Transaction Structures
* Payment transaction structures
* Security and encryption considerations
* Audit trail structures
* Compliance data structures
* Fraud detection structures

### Implementation Patterns
* Secure memory handling
* Sensitive data protection
* Validation and verification
* Audit logging support
* Compliance requirements

## Code Quality Standards

### Documentation
* Explain company specific patterns
* Document performance characteristics
* Note alignment and layout decisions
* Reference production systems
* Provide usage examples

### Performance
* Profile on target hardware
* Measure cache performance
* Benchmark against alternatives
* Document performance characteristics
* Optimize for common access patterns

### Testing
* Test with production like data
* Verify memory layouts
* Test concurrent access patterns
* Validate serialization
* Performance testing with realistic workloads

## Research References
* Google search infrastructure papers
* Bloomberg market data systems
* Uber real time matching algorithms
* Amazon distributed systems papers
* PayPal payment processing systems

## Related Topics
* Performance Optimization: Cache and SIMD optimization
* System Programming: Network and kernel structures
* Advanced Techniques: Metaprogramming for code generation

