# QoS Pipelines

## QoS 0 (At Most Once)
* No acknowledgment; drop on send failure respecting backpressure

## QoS 1 (At Least Once)
* Track inflight with retry timers; retransmit on timeout with DUP set
* Idempotent PUBACK handling

## QoS 2 (Exactly Once)
* State machine: PUBREC → PUBREL → PUBCOMP
* Persist inflight entries; deduplicate on reconnect
* Idempotent handling of duplicate control packets

## Window Management
* Per-client inflight window limits; enforce Receive Maximum (v5)

## Timers and Retries
* Exponential backoff with jitter

## Testing
* Deterministic tests for all transitions and retry paths
* Crash-restart tests to validate deduplication
