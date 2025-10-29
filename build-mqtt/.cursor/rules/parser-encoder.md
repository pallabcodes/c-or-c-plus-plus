# Parser and Encoder Rules

## Fixed Header and Remaining Length
* Parse control packet type and flags; validate per type
* Remaining Length varint: max 4 bytes, detect overflow, reject malformed

## Properties (MQTT 5.0)
* Length-prefixed properties with per type validation
* Enforce single vs repeatable properties; validate ranges

## Zero-Copy Strategy
* Use slices into read buffers; avoid per-packet allocation
* Copy only when retaining or persisting payloads

## Defensive Parsing
* Bound all reads; reject out-of-range values
* Enforce UTF-8 validity where required by spec

## Encoding Rules
* Correct DUP/QoS/RETAIN bit composition
* Minimal allocations; reuse buffers

## Testing
* Fuzz: Remaining Length, properties tables, malformed combinations
* Golden vectors for all control packets
