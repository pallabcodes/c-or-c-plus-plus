# Observability

## Metrics
* Prometheus counters and gauges for connections, inflight, drops, retries
* Histograms for publish latency and end-to-end QoS2 roundtrip

## Tracing
* OpenTelemetry spans around parse→route→send pipeline
* Trace attributes: clientId, topic, qos, payload bytes (size only)

## Logs
* Structured JSON logs with correlation id (per connection/session)
* Distinguish INFO/WARN/ERROR with actionable messages

## $SYS Topics
* Publish broker health and stats under `$SYS` hierarchy

## Dashboards
* Standard panels for connection counts, backpressure, errors, tail latency

## Testing
* Validate metrics presence and cardinality; tracing sampling policies
