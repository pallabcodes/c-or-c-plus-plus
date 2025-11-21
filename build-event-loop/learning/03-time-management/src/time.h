#ifndef TIME_H_
#define TIME_H_

#include <stdint.h>
#include <stddef.h>

/*
 * Time management utilities for event loop.
 * 
 * Provides high-resolution monotonic time for timer management and
 * timeout calculations. Uses monotonic clocks to avoid issues with
 * system clock adjustments.
 * 
 * Reference: node/deps/uv/src/unix/internal.h (uv__update_time)
 *            node/deps/uv/src/unix/posix-hrtime.c (uv__hrtime)
 */

/*
 * Get current time in milliseconds (monotonic clock).
 * 
 * Uses CLOCK_MONOTONIC on Linux/macOS which is not affected by
 * system clock adjustments (NTP, manual changes, etc.).
 * 
 * @return: Current time in milliseconds since an arbitrary point
 */
uint64_t time_now_ms(void);

/*
 * Get current time in nanoseconds (monotonic clock).
 * 
 * Higher precision version for fine-grained timing.
 * 
 * @return: Current time in nanoseconds since an arbitrary point
 */
uint64_t time_now_ns(void);

/*
 * Convert nanoseconds to milliseconds.
 * 
 * @param ns: Time in nanoseconds
 * @return: Time in milliseconds
 */
static inline uint64_t time_ns_to_ms(uint64_t ns) {
  return ns / 1000000;
}

/*
 * Convert milliseconds to nanoseconds.
 * 
 * @param ms: Time in milliseconds
 * @return: Time in nanoseconds
 */
static inline uint64_t time_ms_to_ns(uint64_t ms) {
  return ms * 1000000;
}

/*
 * Calculate time difference (a - b) in milliseconds.
 * 
 * @param a: Later time in milliseconds
 * @param b: Earlier time in milliseconds
 * @return: Difference in milliseconds
 */
static inline uint64_t time_diff_ms(uint64_t a, uint64_t b) {
  return a - b;
}

/*
 * Calculate time difference (a - b) in nanoseconds.
 * 
 * @param a: Later time in nanoseconds
 * @param b: Earlier time in nanoseconds
 * @return: Difference in nanoseconds
 */
static inline uint64_t time_diff_ns(uint64_t a, uint64_t b) {
  return a - b;
}

/*
 * Add milliseconds to a time value.
 * 
 * @param time_ms: Base time in milliseconds
 * @param delta_ms: Milliseconds to add
 * @return: New time in milliseconds
 */
static inline uint64_t time_add_ms(uint64_t time_ms, uint64_t delta_ms) {
  return time_ms + delta_ms;
}

/*
 * Check if time a is before time b.
 * 
 * @param a: First time in milliseconds
 * @param b: Second time in milliseconds
 * @return: 1 if a < b, 0 otherwise
 */
static inline int time_before(uint64_t a, uint64_t b) {
  return a < b;
}

/*
 * Check if time a is after time b.
 * 
 * @param a: First time in milliseconds
 * @param b: Second time in milliseconds
 * @return: 1 if a > b, 0 otherwise
 */
static inline int time_after(uint64_t a, uint64_t b) {
  return a > b;
}

/*
 * Check if a time has passed (is in the past).
 * 
 * @param expiry_time: Expiry time in milliseconds
 * @param now: Current time in milliseconds
 * @return: 1 if expiry_time <= now, 0 otherwise
 */
static inline int time_expired(uint64_t expiry_time, uint64_t now) {
  return expiry_time <= now;
}

#endif /* TIME_H_ */

