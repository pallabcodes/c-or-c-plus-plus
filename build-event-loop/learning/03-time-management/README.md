# Topic 3: Time Management

## What

High-resolution monotonic time utilities for event loop timer management and timeout calculations. Provides millisecond and nanosecond precision time functions using platform-specific monotonic clocks.

## Why

- **Monotonic Clocks**: Not affected by system clock adjustments (NTP, manual changes)
- **High Precision**: Nanosecond precision for accurate timer management
- **Platform Abstraction**: Works across Linux, macOS, Windows
- **Timer Calculations**: Essential for calculating timer expiry and poll timeouts

## Where Used in libuv/Node.js

- Timer expiry calculations: Check if timers are due
- Poll timeout calculation: How long to block in I/O poll
- Loop time updates: Update loop's concept of "now"
- Performance measurements: Track loop iteration times

**Reference**: 
- `node/deps/uv/src/unix/internal.h` (lines 402-406) - `uv__update_time()`
- `node/deps/uv/src/unix/posix-hrtime.c` - `uv__hrtime()`

## Universal Use

Time management is critical in:

- **Event Loops**: All event-driven systems need accurate time
- **Schedulers**: OS schedulers, task schedulers
- **Game Engines**: Frame timing, animation timing
- **Networking**: Timeout management, connection timeouts
- **Databases**: Query timeouts, connection pooling timeouts
- **Real-time Systems**: Deadline scheduling, latency measurements

## Data Structures

```c
/* Time is represented as uint64_t */
uint64_t time_ms;  // Milliseconds
uint64_t time_ns;  // Nanoseconds
```

**Key Characteristics**:
- Monotonic: Always increases, never goes backwards
- High precision: Nanosecond resolution
- Platform-specific: Uses best available clock on each platform

## Algorithms

### Get Current Time: O(1)

**Platform-Specific**:
- **Linux**: `clock_gettime(CLOCK_MONOTONIC)` - Monotonic clock
- **macOS**: `mach_absolute_time()` - High-resolution monotonic time
- **Windows**: `QueryPerformanceCounter()` - High-resolution counter

**Algorithm**:
```
1. Call platform-specific high-resolution time function
2. Convert to nanoseconds (if needed)
3. Return time value
```

### Time Conversion: O(1)

**Nanoseconds to Milliseconds**:
```
ms = ns / 1,000,000
```

**Milliseconds to Nanoseconds**:
```
ns = ms * 1,000,000
```

### Time Arithmetic: O(1)

**Addition**:
```
result = time + delta
```

**Subtraction (Difference)**:
```
diff = later - earlier
```

**Comparison**:
```
before = a < b
after = a > b
expired = expiry <= now
```

## Complexity Analysis

| Operation | Time Complexity | Notes |
|-----------|----------------|-------|
| Get Time | O(1) | System call overhead |
| Conversion | O(1) | Simple arithmetic |
| Arithmetic | O(1) | Simple arithmetic |
| Comparison | O(1) | Simple comparison |

## Implementation Details

### Monotonic vs Real-time Clocks

**Monotonic Clock (CLOCK_MONOTONIC)**:
- Always increases
- Not affected by system clock adjustments
- Perfect for measuring elapsed time
- Used in event loops

**Real-time Clock (CLOCK_REALTIME)**:
- Can go backwards (NTP adjustments)
- Affected by system clock changes
- Used for absolute time (wall clock)
- Not suitable for timers

### Platform-Specific Implementations

**Linux**:
```c
clock_gettime(CLOCK_MONOTONIC, &ts);
return ts.tv_sec * 1e9 + ts.tv_nsec;
```

**macOS**:
```c
mach_timebase_info(&timebase);
uint64_t t = mach_absolute_time();
return t * timebase.numer / timebase.denom;
```

**Windows**:
```c
QueryPerformanceCounter(&counter);
QueryPerformanceFrequency(&frequency);
return counter * 1e9 / frequency;
```

### Precision Considerations

- **Nanoseconds**: Maximum precision, used internally
- **Milliseconds**: Sufficient for most timer operations
- **Conversion Loss**: Nanoseconds → milliseconds loses precision
- **Timer Granularity**: Most systems have ~1ms timer granularity

## Study Notes

### Key Insights from libuv Implementation

1. **Monotonic Clock**: Uses `CLOCK_MONOTONIC` to avoid clock adjustment issues
2. **Millisecond Precision**: Converts nanoseconds to milliseconds for storage
3. **Fast Time Source**: Uses `UV_CLOCK_FAST` when available
4. **Single Update**: Updates loop time once per iteration

### libuv Code Reference

- **File**: `node/deps/uv/src/unix/internal.h` (lines 402-406)
- **Function**: `uv__update_time()`
- **Implementation**: `node/deps/uv/src/unix/posix-hrtime.c`
- **Usage**: Called in `uv_run()` before timer execution

### Time Update Strategy

libuv updates time:
1. At the start of `uv_run()` (if loop not alive)
2. Before initial timer execution (UV_RUN_DEFAULT)
3. After I/O polling (before final timer execution)

This ensures timers see consistent time values throughout an iteration.

## Testing

Run tests:
```bash
cd build-event-loop/learning/03-time-management
gcc -std=c11 -I. -o test_time tests/test_time.c src/time.c
./test_time
```

## Example

See `examples/example.c` for a complete example of using time functions for timer management.

## Next Steps

This time management will be used in:
- Topic 7: Timer Structure (storing expiry times)
- Topic 9: Timer Execution (checking if timers are due)
- Topic 10: Timer Timeout Calculation (calculating poll timeout)
- Topic 22: Loop Initialization (initializing loop time)

## References

- libuv source: `node/deps/uv/src/unix/internal.h`, `node/deps/uv/src/unix/posix-hrtime.c`
- POSIX: `clock_gettime(3)` man page
- Linux: `CLOCK_MONOTONIC` documentation
- macOS: `mach_absolute_time()` documentation

