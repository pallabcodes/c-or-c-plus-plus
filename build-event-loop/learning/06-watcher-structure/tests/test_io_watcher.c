#include <stdio.h>
#include <assert.h>
#include "../src/io_watcher.h"
#include "../04-loop-structure/src/event_loop.h"

static int callback_called = 0;

static void callback(struct event_loop* loop, struct io_watcher* w, unsigned int events) {
  (void)loop;
  (void)w;
  (void)events;
  callback_called = 1;
}

static void test_io_watcher_init(void) {
  struct io_watcher watcher;
  
  callback_called = 0;
  
  io_watcher_init(&watcher, callback, 5);
  
  assert(watcher.cb == callback);
  assert(watcher.fd == 5);
  assert(watcher.events == 0);
  assert(watcher.pevents == 0);
  assert(queue_empty(&watcher.pending_queue));
  assert(queue_empty(&watcher.watcher_queue));
  
  printf("✓ test_io_watcher_init passed\n");
}

static void test_io_watcher_start_stop(void) {
  struct event_loop loop;
  struct io_watcher watcher;
  
  event_loop_init(&loop);
  io_watcher_init(&watcher, NULL, 10);
  
  /* Start watching for read events */
  assert(io_watcher_start(&loop, &watcher, IO_EVENT_READ) == 0);
  assert(io_watcher_active(&watcher, IO_EVENT_READ) == 1);
  assert(watcher.pevents == IO_EVENT_READ);
  assert(!queue_empty(&watcher.watcher_queue));
  
  /* Add write events */
  assert(io_watcher_start(&loop, &watcher, IO_EVENT_WRITE) == 0);
  assert(io_watcher_active(&watcher, IO_EVENT_READ) == 1);
  assert(io_watcher_active(&watcher, IO_EVENT_WRITE) == 1);
  assert(watcher.pevents == (IO_EVENT_READ | IO_EVENT_WRITE));
  
  /* Stop read events */
  io_watcher_stop(&loop, &watcher, IO_EVENT_READ);
  assert(io_watcher_active(&watcher, IO_EVENT_READ) == 0);
  assert(io_watcher_active(&watcher, IO_EVENT_WRITE) == 1);
  assert(watcher.pevents == IO_EVENT_WRITE);
  
  /* Stop write events */
  io_watcher_stop(&loop, &watcher, IO_EVENT_WRITE);
  assert(io_watcher_active(&watcher, IO_EVENT_WRITE) == 0);
  assert(watcher.pevents == 0);
  assert(queue_empty(&watcher.watcher_queue));
  
  event_loop_free(&loop);
  printf("✓ test_io_watcher_start_stop passed\n");
}

static void test_io_watcher_close(void) {
  struct event_loop loop;
  struct io_watcher watcher;
  
  event_loop_init(&loop);
  io_watcher_init(&watcher, NULL, 20);
  
  io_watcher_start(&loop, &watcher, IO_EVENT_READ | IO_EVENT_WRITE);
  assert(io_watcher_active(&watcher, IO_EVENT_READ) == 1);
  
  io_watcher_close(&loop, &watcher);
  assert(watcher.fd == -1);
  assert(watcher.pevents == 0);
  assert(queue_empty(&watcher.watcher_queue));
  assert(queue_empty(&watcher.pending_queue));
  
  event_loop_free(&loop);
  printf("✓ test_io_watcher_close passed\n");
}

static void test_io_watcher_getters(void) {
  struct io_watcher watcher;
  
  io_watcher_init(&watcher, NULL, 42);
  
  assert(io_watcher_get_fd(&watcher) == 42);
  assert(io_watcher_get_pevents(&watcher) == 0);
  assert(io_watcher_get_events(&watcher) == 0);
  
  watcher.pevents = IO_EVENT_READ | IO_EVENT_WRITE;
  watcher.events = IO_EVENT_READ;
  
  assert(io_watcher_get_pevents(&watcher) == (IO_EVENT_READ | IO_EVENT_WRITE));
  assert(io_watcher_get_events(&watcher) == IO_EVENT_READ);
  
  printf("✓ test_io_watcher_getters passed\n");
}

int main(void) {
  printf("Running I/O watcher tests...\n\n");
  
  test_io_watcher_init();
  test_io_watcher_start_stop();
  test_io_watcher_close();
  test_io_watcher_getters();
  
  printf("\nAll I/O watcher tests passed!\n");
  return 0;
}

