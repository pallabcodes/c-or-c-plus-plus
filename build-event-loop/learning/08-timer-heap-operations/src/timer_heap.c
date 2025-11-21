#include "timer_heap.h"
#include <limits.h>
#include <stddef.h>

int timer_heap_next_timeout(const struct event_loop* loop) {
  struct timer* timer;
  uint64_t diff;
  
  if (loop == NULL || loop->timer_heap == NULL) {
    return -1;
  }
  
  /* If heap is empty, block indefinitely */
  if (heap_empty(loop->timer_heap)) {
    return -1;
  }
  
  /* In a min-heap, the minimum is always at index 0 */
  timer = (struct timer*)loop->timer_heap->nodes[0].data;
  
  /* Update loop time to get accurate timeout */
  event_loop_update_time((struct event_loop*)loop);
  
  /* If timer has already expired, return 0 */
  if (timer->timeout <= loop->time) {
    return 0;
  }
  
  /* Calculate time until expiry */
  diff = timer->timeout - loop->time;
  
  /* Clamp to INT_MAX to avoid overflow */
  if (diff > INT_MAX) {
    diff = INT_MAX;
  }
  
  return (int)diff;
}

struct timer* timer_heap_min(const struct event_loop* loop) {
  if (loop == NULL || loop->timer_heap == NULL) {
    return NULL;
  }
  
  if (heap_empty(loop->timer_heap)) {
    return NULL;
  }
  
  /* In a min-heap, the minimum is always at index 0 */
  return (struct timer*)loop->timer_heap->nodes[0].data;
}

int timer_heap_empty(const struct event_loop* loop) {
  if (loop == NULL || loop->timer_heap == NULL) {
    return 1;
  }
  return heap_empty(loop->timer_heap);
}

size_t timer_heap_count(const struct event_loop* loop) {
  if (loop == NULL || loop->timer_heap == NULL) {
    return 0;
  }
  return loop->timer_heap->size;
}

