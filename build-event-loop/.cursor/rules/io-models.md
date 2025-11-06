# IO Models

## Readiness Multiplexing
* epoll on Linux, kqueue on BSD and macOS
* Prefer edge triggered with drain loops; allow level triggered as fallback

## Blocking Hazards
* Never block in reactor; offload to worker pools if needed

## io uring Option
* Consider for high throughput with careful fallback and feature detection

## Error Handling
* Treat EAGAIN and EWOULDBLOCK as normal; handle ECONNRESET and timeouts

## Testing
* Validate edge vs. level behavior and starvation risks
