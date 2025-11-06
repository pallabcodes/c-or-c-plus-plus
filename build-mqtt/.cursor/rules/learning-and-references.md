# Learning and References

## What to learn (topics)
* MQTT 3.1.1 and 5.0 protocol details
* QoS pipelines and state machines
* Session management and expiry
* Topic routing with wildcards and shared subscriptions
* Persistence (WAL, snapshots) and recovery
* Security (TLS, mTLS, ACLs, JWT/OIDC)

## DSA foundations
* Trie or radix tree for topics
* Hash maps for sessions and subscriptions
* Priority queues and wheels for timers
* Lock free queues for IO handoff when justified

## How to improve
* Replace naive topic maps with radix trie and shared sub fair scheduling
* Persist inflight and retained with compaction; reduce fsyncs with batching
* Apply backpressure and slow consumer handling to avoid tail spikes
* Reduce copies via buffer slices and scatter gather writes

## Research papers and usage
* ARIES ideas for recovery: checkpoints plus redo log; adapt to broker state
* Consensus (Raft) for session replication; justify consistency level
* Diff algorithms for retained catalog changes if snapshotting

## Open source to study
* Mosquitto, EMQX, VerneMQ

## Practice plan
* Week 1: v3.1.1 single node with QoS 0/1
* Week 2: QoS 2 and persistence
* Week 3: v5 properties and observability
* Week 4: clustering and security hardening
