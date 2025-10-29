# MQTT Broker Module Overview

## Context
This code is written by an SDE 2 backend and low level system engineer working with top tier product companies including Google, Atlassian, Bloomberg, PayPal, Stripe, Uber, Amazon, and other top tier silicon valley companies. This MQTT broker implementation must meet enterprise production standards suitable for principal level engineering review and be comparable in core quality to open source brokers such as Mosquitto, EMQX, and VerneMQ.

## Purpose
This module covers the design and implementation of a production grade MQTT broker in C and C plus plus. All code must follow production grade standards suitable for principal level code review and must be production ready for deployment in high performance, high availability clustered environments.

## Scope
* Applies to all C and C plus plus code in build mqtt directory
* Extends repository root rules defined in the root `.cursorrules` file
* Covers MQTT 3.1.1 and MQTT 5.0 protocol compliance, QoS 0, 1, and 2, TLS and mTLS security, persistence and crash recovery, and clustered scaling with shared subscriptions
* Code quality standards align with expectations from top tier product companies

## Core Components
1. Protocol and Spec: MQTT 3.1.1 and 5.0 compliance
2. Parser and Encoder: fixed header, varint, properties
3. Session Management: clean start, expiry, inflight tracking, will messages
4. QoS Pipelines: QoS 0, 1, and 2 state machines
5. Routing and Topics: trie with wildcards and shared subscriptions
6. Persistence and Recovery: WAL and snapshots; retained store
7. Clustering and Replication: inter broker transport, sharding, session replication
8. Security: TLS and mTLS, ACLs, JWT/OIDC
9. Observability: `$SYS` topics, metrics, tracing, structured logs
10. Performance: zero copy, batching, reuseport, per core reactors
11. Networking and OS Tuning: TCP options, rlimits, buffers, NUMA
12. Testing and Validation: interop, fuzz, soak, chaos
13. Operations and Admin: config, admin API, tenancy, quotas

## Code Quality Standards
All MQTT code must demonstrate:
* Comprehensive error handling with clear messages and remediation guidance
* Proper resource management with deterministic cleanup and leak freedom
* Correct synchronization to prevent races, deadlocks, and priority inversion
* Memory safety through bounds checking and careful allocation patterns
* Security via TLS everywhere feasible, strong authn/authz, and strict parsing
* Testing of both success and failure scenarios including chaos and fuzz
* Performance optimization with clear targets and measured results

## Reference Material
* MQTT 3.1.1 and MQTT 5.0 specifications
* Open source implementations: Mosquitto, EMQX, VerneMQ
* Networking and OS tuning guides (Linux), OpenSSL documentation

## Related Rules
Refer to other rule files in this directory for protocol details, parsing and encoding, sessions, QoS, routing, persistence and recovery, clustering and replication, security, observability, performance optimization, networking and OS tuning, testing and validation, and operations and admin.
