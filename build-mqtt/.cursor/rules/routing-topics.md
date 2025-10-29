# Routing and Topics

## Topic Trie / Radix Tree
* Implement radix tree with compact edges
* Support `+` and `#` wildcards per spec rules

## Subscriber Tables
* Per-node subscriber lists with QoS caps
* Distinguish shared vs. non-shared subscribers

## Shared Subscriptions
* `$share/<group>/...` groups
* Fair work distribution across group members
* At-least-once delivery guarantees across nodes

## Retained Matching
* Return retained message on fresh SUBSCRIBE

## Performance
* Cache hot path lookups; minimize allocations
* Bounded memory usage with compaction policies

## Testing
* Wildcard correctness matrices
* Shared sub fairness tests
