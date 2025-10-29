# Concurrency Control Standards

## Scope
Applies to all concurrency control code including locking, optimistic concurrency control, and timestamp ordering. Extends repository root rules.

## Locking Protocols

### Two Phase Locking (2PL)
* Growing phase: acquire locks
* Shrinking phase: release locks
* Strict 2PL: release at commit
* Conservative 2PL: acquire all locks upfront
* Reference: "Granularity of Locks and Degrees of Consistency" (Gray et al., 1976)

### Lock Granularity
* Table level locks (coarse)
* Page level locks (medium)
* Row level locks (fine)
* Predicate locks for phantoms
* Multi granularity locking with intention locks

### Lock Modes
* Shared locks for reads
* Exclusive locks for writes
* Update locks for read modify write
* Intention locks for hierarchy
* Schema locks for DDL

## Optimistic Concurrency Control (OCC)

### Validation Protocol
* Read phase: perform operations locally
* Validation phase: check conflicts
* Write phase: commit changes
* Rollback on conflict
* Reference: "Efficient Optimistic Concurrency Control" (Kung & Robinson, 1981)

### Conflict Detection
* Read write conflicts
* Write write conflicts
* Timestamp based validation
* Serializability validation

## Timestamp Ordering (TO)

### Timestamp Assignment
* Transaction start timestamp
* Read and write timestamps for items
* Timestamp comparison for ordering
* Abort and restart on violation

### Basic TO Protocol
* Read check: item write timestamp <= transaction timestamp
* Write check: item read and write timestamps <= transaction timestamp
* Use timestamps for ordering
* Handle cascading aborts

## Multi Version Concurrency Control (MVCC)

### Version Management
* Maintain multiple versions per tuple
* Version chains for traversal
* scan setting timestamp
* Visibility computation
* Reference: "An Empirical Evaluation of In-Memory Multi-Version Concurrency Control" (Wu et al., 2017)

### Snapshot Isolation
* Consistent snapshot per transaction
* No read write conflicts
* First committer wins for write write
* Serializable snapshot isolation (SSI) variant

## Deadlock Handling

### Deadlock Detection
* Wait for graph construction
* Cycle detection algorithm
* Periodic checking
* Distributed deadlock detection

### Deadlock Prevention
* Lock ordering
* Timeout based prevention
* Wait die protocol
* Wound wait protocol

### Deadlock Resolution
* Victim selection (youngest, least progress)
* Rollback and restart
* Avoid starvation
* Notification to application

## Lock Free Data Structures

### Atomic Operations
* Compare and swap (CAS)
* Fetch and add
* Load linked store conditional (LL/SC)
* Memory barriers

### Lock Free Algorithms
* Lock free hash tables
* Lock free queues
* Hazard pointers for memory reclamation
* Read copy update (RCU)

## Implementation Requirements
* Efficient lock table data structure
* Minimal lock contention
* Fast lock acquisition and release
* Support for lock escalation
* Lock statistics and monitoring
* Handle lock timeouts
* Proper lock ordering

## Performance Considerations
* Reduce lock granularity
* Minimize lock duration
* Batch lock operations
* Use lock free structures where possible
* Profile lock contention
* Optimize for read heavy workloads

