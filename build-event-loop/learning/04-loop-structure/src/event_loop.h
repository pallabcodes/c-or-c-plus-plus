#ifndef EVENT_LOOP_H_
#define EVENT_LOOP_H_

#include <stdint.h>
#include <stddef.h>

/* Include dependencies */
#include "../01-intrusive-queue/src/queue.h"
#include "../02-min-heap/src/heap.h"

/*
 * Forward declarations
 */
struct handle;
struct io_watcher;

/*
 * Event loop structure - central state for the entire event loop.
 * 
 * Contains all queues, heaps, watchers, and state needed to run
 * the event loop.
 * 
 * Reference: node/deps/uv/include/uv.h (struct uv_loop_s)
 *            node/deps/uv/include/uv/unix.h (UV_LOOP_PRIVATE_FIELDS)
 */

struct event_loop {
  /* User data - can be used for anything */
  void* data;
  
  /* Timer heap - stores timers sorted by expiry time */
  struct heap* timer_heap;
  
  /* Handle queues - different types of handles */
  struct queue idle_handles;      /* Run on every iteration */
  struct queue prepare_handles;   /* Run before blocking for I/O */
  struct queue check_handles;     /* Run after blocking for I/O */
  struct queue pending_queue;     /* Deferred callbacks */
  struct queue closing_handles;   /* Handles being closed */
  struct queue handle_queue;      /* All active handles */
  struct queue watcher_queue;     /* I/O watchers to be registered */
  
  /* I/O watchers - file descriptor tracking */
  struct io_watcher** watchers;   /* Array of watchers */
  size_t nwatchers;               /* Number of watchers */
  size_t nfds;                    /* Number of file descriptors */
  
  /* Platform-specific I/O polling */
  int backend_fd;                 /* epoll/kqueue file descriptor */
  
  /* Loop state */
  uint64_t time;                  /* Current time in milliseconds */
  unsigned int active_handles;    /* Number of active handles */
  unsigned int stop_flag;         /* Stop flag (1 = stop) */
  
  /* Timer counter - for unique timer IDs */
  uint64_t timer_counter;
};

/*
 * Initialize an empty event loop.
 * 
 * Sets up all queues, initializes timer heap, and prepares
 * platform-specific I/O polling.
 * 
 * @param loop: Loop to initialize
 * @return: 0 on success, -1 on error
 */
int event_loop_init(struct event_loop* loop);

/*
 * Free event loop resources.
 * 
 * @param loop: Loop to free
 */
void event_loop_free(struct event_loop* loop);

/*
 * Check if loop has active work (is alive).
 * 
 * Loop is alive if it has:
 * - Active handles
 * - Active requests
 * - Closing handles
 * 
 * @param loop: Loop to check
 * @return: 1 if alive, 0 otherwise
 */
int event_loop_alive(const struct event_loop* loop);

/*
 * Stop the event loop.
 * 
 * Sets the stop flag, which will cause the loop to exit
 * after the current iteration completes.
 * 
 * @param loop: Loop to stop
 */
void event_loop_stop(struct event_loop* loop);

/*
 * Get current loop time.
 * 
 * @param loop: Loop
 * @return: Current time in milliseconds
 */
static inline uint64_t event_loop_time(const struct event_loop* loop) {
  return loop->time;
}

/*
 * Update loop time.
 * 
 * Should be called periodically to update the loop's concept of "now".
 * 
 * @param loop: Loop
 */
void event_loop_update_time(struct event_loop* loop);

#endif /* EVENT_LOOP_H_ */

