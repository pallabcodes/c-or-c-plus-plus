# Security and Safety

## Input Validation
* Validate sizes, indices, and lengths before use

## Resource Limits
* Bound per connection memory and time spent per tick

## Isolation
* Avoid executing untrusted code in reactor threads; sandbox if needed

## Crash Safety
* Fail fast on invariants; keep reactor consistent

## Logging
* Avoid leaking sensitive payloads in logs
