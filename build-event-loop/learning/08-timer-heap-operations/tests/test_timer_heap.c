#include <stdio.h>
#include <assert.h>
#include "../src/timer_heap.h"
#include "../04-loop-structure/src/event_loop.h"
#include "../07-timer-structure/src/timer.h"

static int callback_count = 0;

static void timer_callback(struct timer* timer) {
  (void)timer;
  callback_count++;
}

static void test_timer_heap_empty(void) {
  struct event_loop loop;
  
  event_loop_init(&loop);
  
  assert(timer_heap_empty(&loop) == 1);
  assert(timer_heap_count(&loop) == 0);
  assert(timer_heap_min(&loop) == NULL);
  
  event_loop_free(&loop);
  printf("✓ test_timer_heap_empty passed\n");
}

static void test_timer_heap_next_timeout(void) {
  struct event_loop loop;
  struct timer timer1, timer2, timer3;
  int timeout;
  
  event_loop_init(&loop);
  
  timer_init(&loop, &timer1);
  timer_init(&loop, &timer2);
  timer_init(&loop, &timer3);
  
  /* No timers - should return -1 */
  timeout = timer_heap_next_timeout(&loop);
  assert(timeout == -1);
  
  /* Add timers with different timeouts */
  timer_start(&timer1, timer_callback, 200, 0);
  timer_start(&timer2, timer_callback, 100, 0);
  timer_start(&timer3, timer_callback, 150, 0);
  
  /* Next timeout should be for timer2 (100ms) */
  timeout = timer_heap_next_timeout(&loop);
  assert(timeout >= 0 && timeout <= 100);
  
  /* Minimum timer should be timer2 */
  struct timer* min_timer = timer_heap_min(&loop);
  assert(min_timer == &timer2);
  assert(min_timer->timeout == timer2.timeout);
  
  /* Count should be 3 */
  assert(timer_heap_count(&loop) == 3);
  
  /* Cleanup */
  timer_stop(&timer1);
  timer_stop(&timer2);
  timer_stop(&timer3);
  
  event_loop_free(&loop);
  printf("✓ test_timer_heap_next_timeout passed\n");
}

static void test_timer_heap_min(void) {
  struct event_loop loop;
  struct timer timer1, timer2, timer3;
  
  event_loop_init(&loop);
  
  timer_init(&loop, &timer1);
  timer_init(&loop, &timer2);
  timer_init(&loop, &timer3);
  
  /* Start timers in different order */
  timer_start(&timer1, timer_callback, 300, 0);
  timer_start(&timer3, timer_callback, 50, 0);
  timer_start(&timer2, timer_callback, 200, 0);
  
  /* Minimum should be timer3 (50ms) */
  struct timer* min_timer = timer_heap_min(&loop);
  assert(min_timer == &timer3);
  assert(min_timer->timeout == timer3.timeout);
  
  /* Remove minimum and check next */
  timer_stop(&timer3);
  min_timer = timer_heap_min(&loop);
  assert(min_timer == &timer2);
  
  /* Cleanup */
  timer_stop(&timer1);
  timer_stop(&timer2);
  
  event_loop_free(&loop);
  printf("✓ test_timer_heap_min passed\n");
}

static void test_timer_heap_count(void) {
  struct event_loop loop;
  struct timer timer1, timer2, timer3;
  
  event_loop_init(&loop);
  
  timer_init(&loop, &timer1);
  timer_init(&loop, &timer2);
  timer_init(&loop, &timer3);
  
  assert(timer_heap_count(&loop) == 0);
  
  timer_start(&timer1, timer_callback, 100, 0);
  assert(timer_heap_count(&loop) == 1);
  
  timer_start(&timer2, timer_callback, 200, 0);
  assert(timer_heap_count(&loop) == 2);
  
  timer_start(&timer3, timer_callback, 300, 0);
  assert(timer_heap_count(&loop) == 3);
  
  timer_stop(&timer2);
  assert(timer_heap_count(&loop) == 2);
  
  timer_stop(&timer1);
  assert(timer_heap_count(&loop) == 1);
  
  timer_stop(&timer3);
  assert(timer_heap_count(&loop) == 0);
  
  event_loop_free(&loop);
  printf("✓ test_timer_heap_count passed\n");
}

int main(void) {
  printf("Running timer heap operations tests...\n\n");
  
  test_timer_heap_empty();
  test_timer_heap_next_timeout();
  test_timer_heap_min();
  test_timer_heap_count();
  
  printf("\nAll timer heap operations tests passed!\n");
  return 0;
}

