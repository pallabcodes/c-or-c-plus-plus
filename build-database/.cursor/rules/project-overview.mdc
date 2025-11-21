# Database Module Overview

## Context
This code is written by an SDE 2 backend and low level system engineer working with top tier product companies including Google, Atlassian, Bloomberg, PayPal, Stripe, Uber, Amazon, and other top tier silicon valley companies. This database implementation must meet enterprise production standards suitable for principal level engineering review and must be comparable to top tier database products like Turso, PlanetScale, PingCAP TiDB, and ClickHouse.

## Purpose
This module covers the design and implementation of a production grade distributed database system in C and C plus plus. All code must follow production grade standards suitable for principal level code review and must be production ready for deployment in high performance, high availability systems handling petabytes of data and millions of queries per second.

## Scope
* Applies to all C and C plus plus code in build database directory
* Extends repository root rules defined in the root `.cursorrules` file
* Covers all aspects of database systems from storage engines to query processing, transaction management, distributed systems, and vector search
* Code quality standards align with expectations from top tier database companies like Turso, PlanetScale, PingCAP, and ClickHouse

## Top Tier Database Product Comparisons

### Turso (libSQL/SQLite Evolution)
Reference: https://turso.tech/
* SQLite compatibility with modern enhancements
* WebAssembly support with OPFS for browser execution
* Native vector search for AI and RAG workflows
* Async I/O using Linux io_uring for responsiveness
* Concurrent writes without traditional locking
* Replication and sync capabilities
* Database branching with copy on write

### PlanetScale (Vitess Based)
Reference: https://planetscale.com/
* Horizontal sharding via Vitess architecture
* VTGate query routing and load balancing
* Multi master replication capabilities
* Online schema migrations with zero downtime
* Database branching for isolated environments
* Global edge network for low latency
* Vector search support for AI workloads

### PingCAP TiDB
* HTAP hybrid transactional and analytical processing
* Raft based replication for distributed consistency
* Distributed transactions across shards
* Columnar storage via TiFlash for analytics
* Vectorized query execution
* JIT compilation for query optimization
* Strong consistency with horizontal scalability

### ClickHouse
* Columnar storage optimized for analytics
* Vectorized query execution engine
* Advanced data compression techniques
* Materialized views for query optimization
* Distributed queries across clusters
* MergeTree engine for time series data
* Real time analytical processing

## Database Architecture Components

### Core Components
1. Storage Engine: B+ trees, LSM trees, columnar storage
2. Query Processor: SQL parsing, planning, optimization, execution
3. Transaction Manager: ACID guarantees, MVCC, isolation levels
4. Recovery System: Write ahead logging, checkpointing, ARIES algorithm
5. Index Manager: Primary and secondary indexes, vector indexes, adaptive structures
6. Concurrency Control: Locking, optimistic concurrency, timestamp ordering
7. Distributed Systems: Consensus algorithms, replication, sharding
8. Vector Search: Embedding storage, similarity search, RAG support
9. Memory Management: Buffer pools, page replacement, memory mapped I/O
10. Network Protocols: Wire protocols, connection pooling, query routing

## Code Quality Standards
All database code must demonstrate:
* Comprehensive error handling with clear messages
* Proper resource management with deterministic cleanup
* Correct synchronization to prevent race conditions and deadlocks
* Memory safety through bounds checking and proper alignment
* Security through encryption, access control, and input validation
* Testing of both success and failure scenarios
* Performance optimization through SIMD, vectorization, and JIT compilation
* Research backed implementations with proper citations

## Reference Material
* See existing examples in system programming directories for low level patterns
* Reference research papers cited in individual rule files
* Study open source implementations of top tier database systems
* Benchmark against TPC standards and industry workloads

## Related Rules
Refer to the other rule files in this directory for specific guidance on storage engines, query processing, transactions, recovery, indexing, concurrency, distributed systems, vector search, performance optimization, memory management, networking, security, and testing.

