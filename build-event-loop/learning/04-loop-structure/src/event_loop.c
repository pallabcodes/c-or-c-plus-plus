#include "event_loop.h"
#include "../01-intrusive-queue/src/queue.h"
#include "../02-min-heap/src/heap.h"
#include "../03-time-management/src/time.h"
#include <stdlib.h>
#include <string.h>

int event_loop_init(struct event_loop* loop) {
  if (loop == NULL)
    return -1;
  
  /* Zero-initialize the loop */
  memset(loop, 0, sizeof(*loop));
  
  /* Initialize timer heap */
  loop->timer_heap = malloc(sizeof(struct heap));
  if (loop->timer_heap == NULL)
    return -1;
  
  if (heap_init(loop->timer_heap, 16) != 0) {
    free(loop->timer_heap);
    return -1;
  }
  
  /* Initialize all queues */
  queue_init(&loop->idle_handles);
  queue_init(&loop->prepare_handles);
  queue_init(&loop->check_handles);
  queue_init(&loop->pending_queue);
  queue_init(&loop->closing_handles);
  queue_init(&loop->handle_queue);
  queue_init(&loop->watcher_queue);
  
  /* Initialize I/O watchers */
  loop->watchers = NULL;
  loop->nwatchers = 0;
  loop->nfds = 0;
  loop->backend_fd = -1;
  
  /* Initialize state */
  loop->active_handles = 0;
  loop->stop_flag = 0;
  loop->timer_counter = 0;
  
  /* Initialize time */
  event_loop_update_time(loop);
  
  return 0;
}

void event_loop_free(struct event_loop* loop) {
  if (loop == NULL)
    return;
  
  if (loop->timer_heap != NULL) {
    heap_free(loop->timer_heap);
    free(loop->timer_heap);
    loop->timer_heap = NULL;
  }
  
  if (loop->watchers != NULL) {
    free(loop->watchers);
    loop->watchers = NULL;
  }
  
  loop->nwatchers = 0;
  loop->nfds = 0;
}

int event_loop_alive(const struct event_loop* loop) {
  if (loop == NULL)
    return 0;
  
  /* Loop is alive if it has active handles or closing handles */
  if (loop->active_handles > 0)
    return 1;
  
  if (!queue_empty(&loop->closing_handles))
    return 1;
  
  /* Also check if timer heap has timers */
  if (loop->timer_heap != NULL && !heap_empty(loop->timer_heap))
    return 1;
  
  return 0;
}

void event_loop_stop(struct event_loop* loop) {
  if (loop != NULL)
    loop->stop_flag = 1;
}

void event_loop_update_time(struct event_loop* loop) {
  if (loop != NULL)
    loop->time = time_now_ms();
}

