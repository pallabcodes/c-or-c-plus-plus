# Observability

## Metrics
* Counters for events processed, drops, retries; histograms for tick time

## Tracing
* Spans around poll→dispatch→callback; attributes include fd, bytes, queue size

## Logs
* Structured logs with thread id and correlation id

## Dashboards
* Panels for queue depth, tick latency, errors

## Testing
* Assert metrics presence and cardinality
