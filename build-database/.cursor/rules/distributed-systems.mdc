# Distributed Database Systems Standards

## Scope
Applies to all distributed database code including consensus, replication, sharding, and distributed transactions. Extends repository root rules.

## Consensus Algorithms

### Raft Consensus
* Leader election
* Log replication
* Safety and liveness guarantees
* Reference: "In Search of an Understandable Consensus Algorithm" (Ongaro & Ousterhout, 2014)
* Implementation used by CockroachDB, TiDB, and others

### Paxos Consensus
* Multi Paxos variant
* Role assignment (proposers, acceptors, learners)
* Quorum requirements
* Reference: "Paxos Made Simple" (Lamport, 2001)
* Foundation for many distributed systems

## Replication

### Replication Strategies

#### Master Slave Replication
* Single write master
* Multiple read replicas
* Asynchronous replication
* Read scaling

#### Multi Master Replication
* Multiple write masters
* Conflict resolution required
* Eventual consistency
* Lower latency writes

### Replication Modes
* Synchronous replication for strong consistency
* Asynchronous replication for lower latency
* Semi synchronous replication as compromise
* Cascading replication for fanout

### Consistency Models
* Strong consistency (linearizability)
* Eventual consistency
* Causal consistency
* Tunable consistency

## Sharding

### Sharding Strategies
* Range based sharding
* Hash based sharding
* Directory based sharding
* Hybrid approaches

### Shard Management
* Shard splitting for growth
* Shard merging for consolidation
* Rebalancing for load distribution
* Hot spot detection and mitigation

### Query Routing
* Routing layer for shard selection
* Cross shard queries
* Aggregation across shards
* Distributed joins

## Distributed Transactions

### Two Phase Commit (2PC)
* Coordinator and participants
* Prepare and commit phases
* Blocking protocol
* Single point of failure

### Three Phase Commit (3PC)
* Non blocking variant
* Pre commit phase
* More robust to failures

### Distributed Transaction Protocols
* Percolator (Google): big table based
* Spanner (Google): TrueTime based
* Reference: "Spanner: Google's Globally-Distributed Database" (Corbett et al., 2012)
* Reference: "F1: A Distributed SQL Database" (Shute et al., 2012)

### Calvin Protocol
* Deterministic locking
* Pre execution phase
* Replication phase
* Reference: "Calvin: Fast Distributed Transactions" (Thomson et al., 2012)

## CAP Theorem Tradeoffs

### Consistency
* Strong consistency requirements
* Linearizability guarantees
* Sequential consistency

### Availability
* System remains operational
* Handle network partitions
* Degrade gracefully

### Partition Tolerance
* Network partition handling
* Split brain prevention
* Partition recovery

## Implementation Requirements
* Handle network failures
* Timeout management
* Retry strategies with backoff
* Idempotent operations
* Distributed clock synchronization
* Monitoring and observability
* Graceful degradation

## Top Tier Database Implementations

### TiDB Architecture
* Raft based replication
* PD (Placement Driver) for metadata
* TiKV for distributed storage
* HTAP capabilities
* Reference: "TiDB: A Raft-based HTAP Database" (Dong et al., 2017)

### CockroachDB Architecture
* Multi version timestamp ordering
* Geo distributed deployments
* Reference: "CockroachDB: The Resilient Geo-Distributed SQL Database" (Taft et al., 2020)

### PlanetScale Vitess
* VTGate for query routing
* Horizontal sharding
* Online schema migrations
* Global edge network

## Performance Considerations
* Minimize cross shard communication
* Batch operations when possible
* Use local reads for read scaling
* Optimize consensus overhead
* Handle network latency
* Profile distributed operations

