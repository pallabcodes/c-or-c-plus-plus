# Architecture and Design

## Model
* Reactor first; evaluate proactor or io uring if justified
* Single thread baseline; per core reactors for scale

## Sharding and Affinity
* Accept sharding with SO_REUSEPORT
* Pin reactors to cores; avoid cross core chatter

## Callback Model
* Non blocking callbacks with bounded work; yield points for long tasks

## State Management
* Explicit reactor state, ready lists, and timer wheels

## Failure Modes
* Handle partial reads writes, EAGAIN, and spurious wakeups

## Testing
* Simulate overload, backlog, and wakeup storms
