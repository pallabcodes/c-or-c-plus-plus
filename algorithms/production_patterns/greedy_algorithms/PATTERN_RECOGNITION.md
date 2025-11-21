# Greedy Algorithms Pattern Recognition

## When to Recognize Greedy Algorithm Opportunity

### Input Characteristics That Suggest Greedy Patterns

1. **Optimal Substructure**
   - Problem can be solved by making locally optimal choices
   - Global optimum can be reached by combining local optima
   - Future decisions don't affect past choices

2. **Greedy Choice Property**
   - Locally optimal choice leads to globally optimal solution
   - No need to reconsider previous decisions
   - One-pass algorithm with immediate commitment

3. **Resource Constraints**
   - Limited resources that must be allocated optimally
   - Time-critical decisions under constraints
   - Resource scheduling and allocation problems

4. **Sequential Decision Making**
   - Decisions made in sequence without backtracking
   - Each choice is final and affects future options
   - State transitions with irreversible actions

## Variant Selection Guide

### Decision Tree

```
Need greedy algorithm?
│
├─ Scheduling/CPU allocation?
│  └─ YES → OS Scheduling Variants
│
├─ Data compression?
│  └─ YES → Huffman Coding
│
├─ Network/graph optimization?
│  └─ YES → Network Routing Variants
│
├─ Database optimization?
│  └─ YES → Query Optimization Greedy
│
├─ Resource allocation?
│  └─ YES → Resource Scheduling Variants
│
├─ Interval/time management?
│  └─ YES → Interval Scheduling
│
└─ General optimization?
   └─ YES → Standard Greedy Variants
```

### Variant Comparison

| Variant | Best For | Key Feature | Time Complexity | Correctness Guarantee |
|---------|----------|-------------|-----------------|----------------------|
| OS Scheduling | CPU/process scheduling | Priority queues, fairness | O(n log n) | Fair scheduling |
| Huffman Coding | Data compression | Frequency-based trees | O(n log n) | Optimal compression |
| Network Routing | Path finding, routing | Shortest path heuristics | O((V+E) log V) | Approximate optimal |
| Query Optimization | Join ordering | Cost-based selection | O(n²) | Good heuristics |
| Interval Scheduling | Time slot allocation | End time sorting | O(n log n) | Optimal for unit intervals |
| Resource Allocation | Memory, bandwidth | Knapsack-like | O(n log n) | Good approximations |

## Detailed Variant Selection

### 1. OS Scheduling Algorithms

**When to Use:**
- Process scheduling in operating systems
- CPU time allocation
- Fair resource sharing
- Real-time system scheduling

**Key Characteristics:**
- Priority-based decision making
- Fairness constraints
- Preemption handling
- Time quantum management

**Real-World Examples:**
- Linux CFS (Completely Fair Scheduler)
- Windows thread scheduler
- Real-time OS schedulers
- Network packet schedulers

### 2. Huffman Coding

**When to Use:**
- Data compression and encoding
- Frequency-based optimization
- Prefix code generation
- Lossless compression algorithms

**Key Characteristics:**
- Frequency analysis
- Binary tree construction
- Prefix-free codes
- Optimal compression ratio

**Real-World Examples:**
- ZIP compression
- JPEG entropy coding
- MP3 audio compression
- Protocol encoding (HTTP/2)

### 3. Network Routing Algorithms

**When to Use:**
- Network path optimization
- Traffic routing
- Shortest path approximations
- Load balancing

**Key Characteristics:**
- Heuristic-based decisions
- Local optimality
- Distributed algorithms
- Real-time routing

**Real-World Examples:**
- OSPF routing protocol
- BGP path selection
- Traffic load balancing
- CDN routing decisions

### 4. Database Query Optimization

**When to Use:**
- Join order selection
- Index selection
- Query plan optimization
- Cost-based optimization

**Key Characteristics:**
- Cost estimation models
- Heuristic selection
- Plan enumeration limits
- Statistics-driven decisions

**Real-World Examples:**
- PostgreSQL query planner
- MySQL optimizer
- Oracle cost-based optimizer
- SQL Server query plans

### 5. Interval Scheduling

**When to Use:**
- Time slot allocation
- Meeting room scheduling
- Resource reservation
- Calendar management

**Key Characteristics:**
- End time prioritization
- Non-overlapping constraints
- Maximum utilization
- Conflict resolution

**Real-World Examples:**
- Calendar applications
- Resource booking systems
- CPU scheduling
- Network bandwidth allocation

## Performance Characteristics

### Time Complexity Comparison

| Variant | Time Complexity | When to Use |
|---------|-----------------|-------------|
| OS Scheduling | O(n log n) | Real-time systems |
| Huffman Coding | O(n log n) | Compression algorithms |
| Network Routing | O((V+E) log V) | Large networks |
| Query Optimization | O(n²) | Complex queries |
| Interval Scheduling | O(n log n) | Resource allocation |
| Resource Allocation | O(n log n) | Knapsack-like problems |

### Approximation Quality

| Variant | Approximation Ratio | When Optimal |
|---------|---------------------|--------------|
| OS Scheduling | Fair (not optimal) | Fairness required |
| Huffman Coding | 1.0 (optimal) | Always optimal |
| Network Routing | Varies (heuristic) | Small networks |
| Query Optimization | Good heuristics | Simple queries |
| Interval Scheduling | 1.0 (optimal) | Unit intervals |
| Resource Allocation | 1.0 (optimal) | Fractional knapsack |

## Use Case Mapping

### Operating System Design
- **Best Choice**: OS Scheduling Algorithms
- **Reason**: Fair resource allocation, real-time constraints
- **Alternatives**: Priority queues for simple scheduling

### Data Compression
- **Best Choice**: Huffman Coding
- **Reason**: Optimal compression, widely used
- **Alternatives**: Arithmetic coding for better compression

### Network Infrastructure
- **Best Choice**: Network Routing Algorithms
- **Reason**: Distributed, scalable routing decisions
- **Alternatives**: Dijkstra for exact shortest paths

### Database Systems
- **Best Choice**: Query Optimization Greedy
- **Reason**: Cost-based heuristics work well in practice
- **Alternatives**: Dynamic programming for small query sets

### Resource Management
- **Best Choice**: Interval Scheduling
- **Reason**: Optimal for non-overlapping resources
- **Alternatives**: Graph coloring for complex constraints

## Key Patterns Extracted

### Pattern 1: Priority-Based Selection
- **Found in**: OS schedulers, network routing
- **Technique**: Choose highest priority item first
- **Benefit**: Simple implementation, good performance
- **Trade-off**: May not be globally optimal

### Pattern 2: Frequency Analysis
- **Found in**: Huffman coding, compression algorithms
- **Technique**: Sort by frequency/probability
- **Benefit**: Optimal for entropy-based problems
- **Trade-off**: Requires frequency computation

### Pattern 3: End Time Sorting
- **Found in**: Interval scheduling, resource allocation
- **Technique**: Sort by finish time, greedy selection
- **Benefit**: Optimal for unit intervals
- **Trade-off**: Limited to specific constraint types

### Pattern 4: Cost-Based Heuristics
- **Found in**: Database optimization, query planning
- **Technique**: Estimate costs, choose lowest cost option
- **Benefit**: Good practical performance
- **Trade-off**: Heuristic, not always optimal

### Pattern 5: Greedy Graph Algorithms
- **Found in**: Network routing, minimum spanning trees
- **Technique**: Add lowest cost edge that doesn't form cycle
- **Benefit**: Efficient, good approximations
- **Trade-off**: Not always optimal (but often close)

## Real-World Examples

### Linux Kernel Scheduling
- **Pattern**: Completely Fair Scheduler (CFS)
- **Usage**: Process scheduling with fairness
- **Why**: Balances interactivity and throughput

### Network Protocols
- **Pattern**: OSPF Dijkstra with heuristics
- **Usage**: Internet routing decisions
- **Why**: Scalable routing in large networks

### Database Engines
- **Pattern**: Cost-based join order selection
- **Usage**: Query optimization in RDBMS
- **Why**: Fast query planning for complex joins

### Compression Libraries
- **Pattern**: Huffman coding with frequency analysis
- **Usage**: ZIP, GZIP, image compression
- **Why**: Optimal entropy coding

## References

### Production Codebases
- Linux Kernel: https://github.com/torvalds/linux
- PostgreSQL: https://github.com/postgres/postgres
- Zlib (compression): https://github.com/madler/zlib

### Research Papers
- "Completely Fair Scheduler" - Linux kernel documentation
- "Huffman Coding" - Information theory papers
- "Query Optimization" - Database research papers

### Books and Textbooks
- "Operating System Concepts" - Scheduling algorithms
- "Introduction to Algorithms" (CLRS) - Greedy algorithms
- "Database System Concepts" - Query optimization

### Online Resources
- Linux Scheduler Documentation
- Network routing RFCs
- Database query optimization papers
