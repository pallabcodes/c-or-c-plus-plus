#include <stdio.h>
#include "../src/io_watcher.h"
#include "../04-loop-structure/src/event_loop.h"

/*
 * Example: Using I/O watcher structure.
 * 
 * This demonstrates how I/O watchers track file descriptors
 * for I/O events in the event loop.
 */

static void read_callback(struct event_loop* loop,
                          struct io_watcher* watcher,
                          unsigned int events) {
  (void)loop;
  
  printf("Read callback called for fd=%d, events=0x%x\n",
         io_watcher_get_fd(watcher), events);
  
  if (events & IO_EVENT_READ) {
    printf("  Data available for reading\n");
  }
  if (events & IO_EVENT_WRITE) {
    printf("  Ready for writing\n");
  }
  if (events & IO_EVENT_ERROR) {
    printf("  Error condition\n");
  }
}

int main(void) {
  struct event_loop loop;
  struct io_watcher watcher1, watcher2;
  
  /* Initialize event loop */
  event_loop_init(&loop);
  
  /* Initialize watchers for different file descriptors */
  io_watcher_init(&watcher1, read_callback, 5);
  io_watcher_init(&watcher2, read_callback, 10);
  
  printf("Watcher Information:\n");
  printf("  Watcher 1: fd=%d, events=%u, pevents=%u\n",
         io_watcher_get_fd(&watcher1),
         io_watcher_get_events(&watcher1),
         io_watcher_get_pevents(&watcher1));
  
  /* Start watching for read events on watcher1 */
  io_watcher_start(&loop, &watcher1, IO_EVENT_READ);
  printf("\nStarted watching fd=%d for READ events\n",
         io_watcher_get_fd(&watcher1));
  printf("  Active for READ: %d\n",
         io_watcher_active(&watcher1, IO_EVENT_READ));
  
  /* Start watching for read and write events on watcher2 */
  io_watcher_start(&loop, &watcher2, IO_EVENT_READ | IO_EVENT_WRITE);
  printf("\nStarted watching fd=%d for READ and WRITE events\n",
         io_watcher_get_fd(&watcher2));
  printf("  Active for READ: %d\n",
         io_watcher_active(&watcher2, IO_EVENT_READ));
  printf("  Active for WRITE: %d\n",
         io_watcher_active(&watcher2, IO_EVENT_WRITE));
  
  /* Stop watching for write events on watcher2 */
  io_watcher_stop(&loop, &watcher2, IO_EVENT_WRITE);
  printf("\nStopped watching fd=%d for WRITE events\n",
         io_watcher_get_fd(&watcher2));
  printf("  Active for READ: %d\n",
         io_watcher_active(&watcher2, IO_EVENT_READ));
  printf("  Active for WRITE: %d\n",
         io_watcher_active(&watcher2, IO_EVENT_WRITE));
  
  /* Close watcher1 */
  io_watcher_close(&loop, &watcher1);
  printf("\nClosed watcher1\n");
  printf("  fd=%d (should be -1)\n", io_watcher_get_fd(&watcher1));
  
  /* Cleanup */
  event_loop_free(&loop);
  
  return 0;
}

