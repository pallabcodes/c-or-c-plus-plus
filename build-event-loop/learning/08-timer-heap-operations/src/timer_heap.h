#ifndef TIMER_HEAP_H_
#define TIMER_HEAP_H_

#include <stdint.h>
#include "../02-min-heap/src/heap.h"
#include "../04-loop-structure/src/event_loop.h"
#include "../07-timer-structure/src/timer.h"

/*
 * Timer Heap Operations - Helper functions for timer heap management.
 * 
 * Provides high-level operations for working with timers in the heap:
 * - Finding next timer timeout
 * - Peeking at minimum timer
 * - Helper functions for timer execution
 * 
 * Based on libuv's timer heap operations.
 * 
 * Source: node/deps/uv/src/timer.c (lines 144-195)
 */

/* Forward declaration */
struct event_loop;

/*
 * Get the next timer timeout in milliseconds.
 * 
 * Returns the time until the next timer expires, or -1 if no timers.
 * Returns 0 if a timer has already expired.
 * 
 * @param loop: Event loop
 * @return: Milliseconds until next timer, -1 if no timers, 0 if timer expired
 */
int timer_heap_next_timeout(const struct event_loop* loop);

/*
 * Get the minimum timer from the heap (without removing it).
 * 
 * @param loop: Event loop
 * @return: Pointer to minimum timer, or NULL if heap is empty
 */
struct timer* timer_heap_min(const struct event_loop* loop);

/*
 * Check if timer heap is empty.
 * 
 * @param loop: Event loop
 * @return: 1 if empty, 0 otherwise
 */
int timer_heap_empty(const struct event_loop* loop);

/*
 * Get count of active timers in heap.
 * 
 * @param loop: Event loop
 * @return: Number of timers in heap
 */
size_t timer_heap_count(const struct event_loop* loop);

#endif /* TIMER_HEAP_H_ */

