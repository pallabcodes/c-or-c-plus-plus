#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>
#include "../01-intrusive-queue/src/queue.h"
#include "../02-min-heap/src/heap.h"
#include "../05-handle-structure/src/handle.h"
#include "../04-loop-structure/src/event_loop.h"

/*
 * Timer Structure - Timer handle with expiry time and callback.
 * 
 * Timers are used to schedule callbacks for future execution.
 * They are stored in a min-heap sorted by expiry time.
 * 
 * Based on libuv's uv_timer_t structure.
 * 
 * Source: node/deps/uv/src/timer.c
 *         node/deps/uv/include/uv.h (lines 965-968)
 */

/* Forward declaration */
struct event_loop;
struct timer;

/* Timer callback type */
typedef void (*timer_cb)(struct timer* timer);

/* Timer structure - inherits from handle */
struct timer {
  /* Base handle structure */
  struct handle handle;
  
  /* Timer callback */
  timer_cb timer_cb;
  
  /* Heap index for timer heap (index in heap array) */
  size_t heap_index;
  
  /* Queue node for ready queue (when timer expires) */
  struct queue queue_node;
  
  /* Timer fields */
  uint64_t timeout;      /* Absolute expiry time (milliseconds) */
  uint64_t repeat;      /* Repeat interval (0 = one-shot) */
  uint64_t start_id;    /* Unique ID for ordering (for same timeout) */
};

/* Initialize a timer */
int timer_init(struct event_loop* loop, struct timer* timer);

/* Start a timer */
int timer_start(struct timer* timer,
                timer_cb cb,
                uint64_t timeout,    /* Relative timeout in milliseconds */
                uint64_t repeat);    /* Repeat interval (0 = one-shot) */

/* Stop a timer */
int timer_stop(struct timer* timer);

/* Restart a timer with its repeat interval */
int timer_again(struct timer* timer);

/* Set repeat interval */
void timer_set_repeat(struct timer* timer, uint64_t repeat);

/* Get repeat interval */
uint64_t timer_get_repeat(const struct timer* timer);

/* Get time until timer expires (0 if already expired) */
uint64_t timer_get_due_in(const struct timer* timer);

/* Get timer timeout */
uint64_t timer_get_timeout(const struct timer* timer);

/* Check if timer is active */
int timer_is_active(const struct timer* timer);

#endif /* TIMER_H_ */

