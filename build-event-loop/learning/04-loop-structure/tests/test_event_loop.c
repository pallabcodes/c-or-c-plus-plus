#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "../src/event_loop.h"

static void test_event_loop_init(void) {
  struct event_loop loop;
  
  assert(event_loop_init(&loop) == 0);
  assert(loop.timer_heap != NULL);
  assert(loop.active_handles == 0);
  assert(loop.stop_flag == 0);
  assert(loop.time > 0);
  
  event_loop_free(&loop);
  printf("PASS: test_event_loop_init\n");
}

static void test_event_loop_alive(void) {
  struct event_loop loop;
  
  assert(event_loop_init(&loop) == 0);
  
  /* Empty loop should not be alive */
  assert(event_loop_alive(&loop) == 0);
  
  /* Loop with active handles should be alive */
  loop.active_handles = 1;
  assert(event_loop_alive(&loop) == 1);
  
  event_loop_free(&loop);
  printf("PASS: test_event_loop_alive\n");
}

static void test_event_loop_stop(void) {
  struct event_loop loop;
  
  assert(event_loop_init(&loop) == 0);
  assert(loop.stop_flag == 0);
  
  event_loop_stop(&loop);
  assert(loop.stop_flag == 1);
  
  event_loop_free(&loop);
  printf("PASS: test_event_loop_stop\n");
}

static void test_event_loop_time(void) {
  struct event_loop loop;
  uint64_t t1, t2;
  
  assert(event_loop_init(&loop) == 0);
  
  t1 = event_loop_time(&loop);
  assert(t1 > 0);
  
  event_loop_update_time(&loop);
  t2 = event_loop_time(&loop);
  assert(t2 >= t1);
  
  event_loop_free(&loop);
  printf("PASS: test_event_loop_time\n");
}

int main(void) {
  printf("Running event loop structure tests...\n\n");
  
  test_event_loop_init();
  test_event_loop_alive();
  test_event_loop_stop();
  test_event_loop_time();
  
  printf("\nAll tests passed!\n");
  return 0;
}

