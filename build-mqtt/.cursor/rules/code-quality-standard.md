# Code Quality Standards for MQTT Broker

## Scope
Applies to all code in the MQTT broker module. Extends repository root rules.

## Review Context
Assume line by line scrutiny by principal level reviewers from top tier companies. Code must be production ready for high scale, low latency, and high availability environments.

## Readability
* Self documenting names; avoid abbreviations not in the protocol or OS domain
* Explicit units for time and sizes (ms, ns, bytes)
* Group related logic and separate concerns (parsing vs. state updates vs. I/O)
* Minimize implicit state; prefer explicit data flow

## Maintainability
* Single responsibility per function; avoid deep nesting; use early returns
* Clear separation of protocol, routing, persistence, clustering, and ops concerns
* Use constants and configuration for thresholds; no magic numbers
* Document invariants: inflight counts, window limits, timers

## Debuggability
* Actionable error messages with protocol context (packet type, client id, pid)
* Structured logging with correlation ids per connection and session
* Distinguish transient vs. permanent errors; expose counters and gauges

## Size Constraints
* Maximum function length: 50 lines including comments and whitespace
* Maximum file length: 200 lines including comments and whitespace
* Maximum cyclomatic complexity per function: 10

## Performance
* Profile critical paths (parse, route, QoS pipelines, network I/O)
* Bounded allocations in hot paths; preallocate where safe
* Prefer zero copy buffers and scatter gather I O for writes
* Avoid per packet heap churn; use pools and slabs

## Security
* Strict parsing with bounds checks; fail closed on malformed packets
* TLS everywhere feasible; secure defaults for ciphers and versions
* AuthN/AuthZ per tenant with least privilege and deny by default

## Testing
* Interop with Paho/Mosquitto clients (3.1.1 and 5.0)
* Fuzz parsers and state machines; chaos tests for crash and partition
* Deterministic tests for QoS state machines and timers

## Operations
* Safe config reload with validation and staged apply
* Exhaustive metrics for backpressure, inflight, drops, and retries
* `$SYS` topics reflect broker health and resource usage

## Documentation
* Each public API and data structure documents invariants and lifetimes
* Each protocol handler documents transitions, timers, and side effects
