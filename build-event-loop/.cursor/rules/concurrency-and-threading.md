# Concurrency and Threading

## Threading Model
* One reactor thread per core; optional worker pool for blocking tasks

## Affinity
* Pin threads; align queues and data to cache lines

## Synchronization
* Avoid shared state; use message passing and ownership transfer

## Hazards
* Priority inversion, ABA in lock free structures; add guards

## Testing
* Stress under load; detect races and deadlocks
