#ifndef IO_WATCHER_H_
#define IO_WATCHER_H_

#include <stddef.h>
#include "../01-intrusive-queue/src/queue.h"
#include "../04-loop-structure/src/event_loop.h"

/*
 * I/O Watcher Structure - File descriptor watcher for I/O events.
 * 
 * Tracks which file descriptors to monitor for I/O events (read, write, etc.)
 * Used by the I/O polling system (epoll/kqueue) to know which FDs to watch.
 * 
 * Based on libuv's uv__io_t structure.
 * 
 * Source: node/deps/uv/src/unix/core.c (lines 906-914)
 *         node/deps/uv/src/unix/internal.h (line 259)
 */

/* Forward declaration */
struct event_loop;

/* I/O event flags */
#define IO_EVENT_READ   0x01  /* POLLIN - Data available for reading */
#define IO_EVENT_WRITE  0x02  /* POLLOUT - Ready for writing */
#define IO_EVENT_ERROR  0x04  /* POLLERR - Error condition */
#define IO_EVENT_HUP    0x08  /* POLLHUP - Hang up */

/* I/O callback type */
typedef void (*io_watcher_cb)(struct event_loop* loop,
                               struct io_watcher* watcher,
                               unsigned int events);

/* I/O watcher structure */
struct io_watcher {
  /* Queue nodes */
  struct queue pending_queue;   /* Queue node for pending callbacks */
  struct queue watcher_queue;   /* Queue node for watcher queue */
  
  /* Callback function */
  io_watcher_cb cb;             /* Callback to call when events occur */
  
  /* File descriptor */
  int fd;                       /* File descriptor to watch */
  
  /* Event masks */
  unsigned int events;          /* Current events (registered with poller) */
  unsigned int pevents;         /* Pending events (to be registered) */
};

/* Initialize an I/O watcher */
void io_watcher_init(struct io_watcher* watcher,
                     io_watcher_cb cb,
                     int fd);

/* Start watching for events */
int io_watcher_start(struct event_loop* loop,
                     struct io_watcher* watcher,
                     unsigned int events);

/* Stop watching for events */
void io_watcher_stop(struct event_loop* loop,
                     struct io_watcher* watcher,
                     unsigned int events);

/* Close watcher (stop all events) */
void io_watcher_close(struct event_loop* loop,
                      struct io_watcher* watcher);

/* Check if watcher is active for given events */
int io_watcher_active(const struct io_watcher* watcher, unsigned int events);

/* Get file descriptor */
int io_watcher_get_fd(const struct io_watcher* watcher);

/* Get pending events */
unsigned int io_watcher_get_pevents(const struct io_watcher* watcher);

/* Get current events */
unsigned int io_watcher_get_events(const struct io_watcher* watcher);

#endif /* IO_WATCHER_H_ */

