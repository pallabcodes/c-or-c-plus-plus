# Timers and Scheduling

## Timers
* Hierarchical timer wheel for large timer counts; min heap for low counts
* Coalesce timers to reduce jitter and wakeups

## Scheduling
* Fair run queue per reactor; priorities optional
* Budget work per tick to avoid tail latency

## Cancellation
* Efficient cancellation and reschedule operations

## Testing
* Deterministic timer tests; drift and jitter bounds
