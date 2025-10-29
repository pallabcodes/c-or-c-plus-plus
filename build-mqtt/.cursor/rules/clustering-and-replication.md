# Clustering and Replication

## Sharding
* Partition by clientId or topic hash; define ownership and migration rules
* Sticky sessions to home shard; reconnect migration on failure

## Inter-Broker Transport
* gRPC or custom framed TCP with length-prefix
* Idempotent forwards; deduplication keys for at-least-once

## Session Replication
* Primary/replica per shard or consensus (Raft) for durable sessions
* Snapshot and log compaction policies

## Shared Subscriptions Across Nodes
* Fair work distribution; backpressure aware scheduling
* Preserve at-least-once; QoS2 exactly-once via session inflight replication

## Failure Handling
* Node kill, partitions, rolling restarts; leadership transfer

## Testing
* Chaos tests: packet loss/latency; partitions and heal
* Consistency checks for retained and subscription catalogs
