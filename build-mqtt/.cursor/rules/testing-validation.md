# Testing and Validation

## Interoperability
* Test with Eclipse Paho and Mosquitto clients (3.1.1 and 5.0)

## Fuzzing
* Fuzz control packet parsers and QoS state machines

## Soak and Chaos
* 24–72 hour soaks with churn; packet loss/latency; broker kill -9; disk full/slow

## Conformance
* Build conformance matrices for v3.1.1 and v5 features

## Deterministic Tests
* QoS transitions and timers; session expiry and will semantics

## Observability Validation
* Assert metrics/traces/logs presence and cardinality
