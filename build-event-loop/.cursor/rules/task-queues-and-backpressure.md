# Task Queues and Backpressure

## Queues
* SPSC for reactor to worker; MPSC for workers to reactor
* Ring buffers with fixed capacity

## Backpressure
* High and low watermarks; drop or shed policies for overload
* Slow consumer handling and fairness

## Work Handoff
* Avoid ping pong; batch submissions

## Testing
* Overload tests; watermark behavior and recovery
