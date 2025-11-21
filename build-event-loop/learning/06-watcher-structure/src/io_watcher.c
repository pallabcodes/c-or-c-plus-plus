#include "io_watcher.h"
#include <string.h>
#include <assert.h>

void io_watcher_init(struct io_watcher* watcher,
                     io_watcher_cb cb,
                     int fd) {
  if (watcher == NULL) {
    return;
  }
  
  assert(fd >= -1);
  
  memset(watcher, 0, sizeof(struct io_watcher));
  
  queue_init(&watcher->pending_queue);
  queue_init(&watcher->watcher_queue);
  
  watcher->cb = cb;
  watcher->fd = fd;
  watcher->events = 0;
  watcher->pevents = 0;
}

int io_watcher_start(struct event_loop* loop,
                     struct io_watcher* watcher,
                     unsigned int events) {
  if (loop == NULL || watcher == NULL) {
    return -1;
  }
  
  /* Validate events */
  if (events == 0) {
    return -1;
  }
  
  if (watcher->fd < 0) {
    return -1;
  }
  
  /* Add events to pending events */
  watcher->pevents |= events;
  
  /* If not already in watcher queue, add it */
  if (queue_empty(&watcher->watcher_queue)) {
    queue_insert_tail(&loop->watcher_queue, &watcher->watcher_queue);
  }
  
  /* TODO: Register with platform-specific poller (epoll/kqueue) */
  /* This will be implemented in Topics 17-21 */
  
  return 0;
}

void io_watcher_stop(struct event_loop* loop,
                     struct io_watcher* watcher,
                     unsigned int events) {
  if (loop == NULL || watcher == NULL) {
    return;
  }
  
  if (watcher->fd < 0) {
    return;
  }
  
  /* Remove events from pending events */
  watcher->pevents &= ~events;
  
  /* If no pending events, remove from watcher queue */
  if (watcher->pevents == 0) {
    queue_remove(&watcher->watcher_queue);
    queue_init(&watcher->watcher_queue);
    watcher->events = 0;
    
    /* TODO: Unregister from platform-specific poller */
  } else if (queue_empty(&watcher->watcher_queue)) {
    /* Still has pending events, re-add to queue */
    queue_insert_tail(&loop->watcher_queue, &watcher->watcher_queue);
  }
}

void io_watcher_close(struct event_loop* loop,
                      struct io_watcher* watcher) {
  if (loop == NULL || watcher == NULL) {
    return;
  }
  
  /* Stop all events */
  io_watcher_stop(loop, watcher, IO_EVENT_READ | IO_EVENT_WRITE | 
                                  IO_EVENT_ERROR | IO_EVENT_HUP);
  
  /* Remove from pending queue */
  queue_remove(&watcher->pending_queue);
  queue_init(&watcher->pending_queue);
  
  /* Reset file descriptor */
  watcher->fd = -1;
}

int io_watcher_active(const struct io_watcher* watcher, unsigned int events) {
  if (watcher == NULL) {
    return 0;
  }
  return (watcher->pevents & events) != 0;
}

int io_watcher_get_fd(const struct io_watcher* watcher) {
  if (watcher == NULL) {
    return -1;
  }
  return watcher->fd;
}

unsigned int io_watcher_get_pevents(const struct io_watcher* watcher) {
  if (watcher == NULL) {
    return 0;
  }
  return watcher->pevents;
}

unsigned int io_watcher_get_events(const struct io_watcher* watcher) {
  if (watcher == NULL) {
    return 0;
  }
  return watcher->events;
}

