#include "timer.h"
#include <string.h>
#include <assert.h>
#include <stddef.h>

int timer_init(struct event_loop* loop, struct timer* timer) {
  if (loop == NULL || timer == NULL) {
    return -1;
  }
  
  /* Initialize base handle */
  handle_init(&timer->handle, loop, HANDLE_TYPE_TIMER);
  
  /* Initialize timer-specific fields */
  timer->timer_cb = NULL;
  timer->timeout = 0;
  timer->repeat = 0;
  timer->start_id = 0;
  timer->heap_index = SIZE_MAX;  /* Invalid index */
  
  /* Initialize queue node */
  queue_init(&timer->queue_node);
  
  return 0;
}

int timer_start(struct timer* timer,
                timer_cb cb,
                uint64_t timeout,
                uint64_t repeat) {
  struct event_loop* loop;
  uint64_t clamped_timeout;
  
  if (timer == NULL || cb == NULL) {
    return -1;
  }
  
  loop = handle_get_loop(&timer->handle);
  if (loop == NULL) {
    return -1;
  }
  
  /* Check if handle is closing */
  if (handle_is_closing(&timer->handle)) {
    return -1;
  }
  
  /* Stop timer if already active */
  timer_stop(timer);
  
  /* Calculate absolute timeout */
  event_loop_update_time(loop);
  clamped_timeout = loop->time + timeout;
  
  /* Check for overflow */
  if (clamped_timeout < timeout) {
    clamped_timeout = UINT64_MAX;
  }
  
  /* Set timer fields */
  timer->timer_cb = cb;
  timer->timeout = clamped_timeout;
  timer->repeat = repeat;
  timer->start_id = loop->timer_counter++;
  
  /* Insert into timer heap */
  if (loop->timer_heap != NULL) {
    /* Use timeout as key, timer pointer as data */
    if (heap_insert(loop->timer_heap, timer->timeout, timer) == 0) {
      /* Get heap index (we'll need to track this) */
      /* For now, we'll find it by searching - not ideal but works */
      void* data;
      uint64_t key;
      size_t i;
      for (i = 0; i < loop->timer_heap->size; i++) {
        if (loop->timer_heap->nodes[i].data == timer) {
          timer->heap_index = i;
          break;
        }
      }
    }
  }
  
  /* Mark handle as active */
  handle_set_active(&timer->handle);
  
  return 0;
}

int timer_stop(struct timer* timer) {
  struct event_loop* loop;
  
  if (timer == NULL) {
    return -1;
  }
  
  loop = handle_get_loop(&timer->handle);
  if (loop == NULL) {
    return -1;
  }
  
  /* If active, remove from heap */
  if (handle_is_active(&timer->handle)) {
    if (loop->timer_heap != NULL && timer->heap_index != SIZE_MAX) {
      /* Find timer in heap and remove */
      size_t i;
      for (i = 0; i < loop->timer_heap->size; i++) {
        if (loop->timer_heap->nodes[i].data == timer) {
          heap_remove(loop->timer_heap, i);
          break;
        }
      }
      timer->heap_index = SIZE_MAX;
    }
    handle_set_inactive(&timer->handle);
  } else {
    /* Not active, remove from queue if present */
    queue_remove(&timer->queue_node);
  }
  
  /* Initialize queue node */
  queue_init(&timer->queue_node);
  
  return 0;
}

int timer_again(struct timer* timer) {
  if (timer == NULL || timer->timer_cb == NULL) {
    return -1;
  }
  
  /* If repeat is set, restart timer with repeat interval */
  if (timer->repeat != 0) {
    timer_stop(timer);
    return timer_start(timer, timer->timer_cb, timer->repeat, timer->repeat);
  }
  
  return 0;
}

void timer_set_repeat(struct timer* timer, uint64_t repeat) {
  if (timer != NULL) {
    timer->repeat = repeat;
  }
}

uint64_t timer_get_repeat(const struct timer* timer) {
  if (timer == NULL) {
    return 0;
  }
  return timer->repeat;
}

uint64_t timer_get_due_in(const struct timer* timer) {
  struct event_loop* loop;
  
  if (timer == NULL) {
    return 0;
  }
  
  loop = handle_get_loop(&timer->handle);
  if (loop == NULL) {
    return 0;
  }
  
  /* Update loop time */
  event_loop_update_time(loop);
  
  /* If already expired, return 0 */
  if (loop->time >= timer->timeout) {
    return 0;
  }
  
  return timer->timeout - loop->time;
}

uint64_t timer_get_timeout(const struct timer* timer) {
  if (timer == NULL) {
    return 0;
  }
  return timer->timeout;
}

int timer_is_active(const struct timer* timer) {
  if (timer == NULL) {
    return 0;
  }
  return handle_is_active(&timer->handle);
}

