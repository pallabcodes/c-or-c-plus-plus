# Testing and Validation

## Determinism
* Timers and scheduling must be testable deterministically

## Soak
* 24–72 hour soaks with churn; ensure no leaks and stable latency

## Fuzz
* Fuzz event sources and callback paths

## Concurrency
* Stress with many connections and small payloads; fairness checks

## Regression
* Record and compare flamegraphs on key scenarios
