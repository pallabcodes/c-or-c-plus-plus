#include <stdio.h>
#include <assert.h>
#include "../src/timer.h"
#include "../04-loop-structure/src/event_loop.h"

static int callback_count = 0;

static void timer_callback(struct timer* timer) {
  (void)timer;
  callback_count++;
}

static void test_timer_init(void) {
  struct event_loop loop;
  struct timer timer;
  
  event_loop_init(&loop);
  assert(timer_init(&loop, &timer) == 0);
  
  assert(timer.timer_cb == NULL);
  assert(timer.timeout == 0);
  assert(timer.repeat == 0);
  assert(timer.start_id == 0);
  assert(handle_get_type(&timer.handle) == HANDLE_TYPE_TIMER);
  assert(handle_get_loop(&timer.handle) == &loop);
  assert(!timer_is_active(&timer));
  
  event_loop_free(&loop);
  printf("✓ test_timer_init passed\n");
}

static void test_timer_start_stop(void) {
  struct event_loop loop;
  struct timer timer;
  
  event_loop_init(&loop);
  timer_init(&loop, &timer);
  
  /* Start timer */
  assert(timer_start(&timer, timer_callback, 100, 0) == 0);
  assert(timer_is_active(&timer) == 1);
  assert(timer.timer_cb == timer_callback);
  assert(timer.repeat == 0);
  assert(timer.timeout > loop.time);
  
  /* Stop timer */
  assert(timer_stop(&timer) == 0);
  assert(timer_is_active(&timer) == 0);
  
  event_loop_free(&loop);
  printf("✓ test_timer_start_stop passed\n");
}

static void test_timer_repeat(void) {
  struct event_loop loop;
  struct timer timer;
  
  event_loop_init(&loop);
  timer_init(&loop, &timer);
  
  /* Start timer with repeat */
  assert(timer_start(&timer, timer_callback, 100, 50) == 0);
  assert(timer.repeat == 50);
  
  /* Change repeat */
  timer_set_repeat(&timer, 200);
  assert(timer_get_repeat(&timer) == 200);
  
  event_loop_free(&loop);
  printf("✓ test_timer_repeat passed\n");
}

static void test_timer_due_in(void) {
  struct event_loop loop;
  struct timer timer;
  uint64_t due_in;
  
  event_loop_init(&loop);
  timer_init(&loop, &timer);
  
  /* Start timer for 100ms */
  assert(timer_start(&timer, timer_callback, 100, 0) == 0);
  
  /* Get time until expiry */
  due_in = timer_get_due_in(&timer);
  assert(due_in > 0 && due_in <= 100);
  
  /* Stop timer */
  timer_stop(&timer);
  
  event_loop_free(&loop);
  printf("✓ test_timer_due_in passed\n");
}

static void test_timer_again(void) {
  struct event_loop loop;
  struct timer timer;
  uint64_t timeout1, timeout2;
  
  event_loop_init(&loop);
  timer_init(&loop, &timer);
  
  /* Start timer with repeat */
  assert(timer_start(&timer, timer_callback, 100, 50) == 0);
  timeout1 = timer.timeout;
  assert(timer.repeat == 50);
  
  /* Stop and restart with again */
  timer_stop(&timer);
  assert(timer_again(&timer) == 0);
  timeout2 = timer.timeout;
  
  /* Should have restarted with repeat interval */
  assert(timer_is_active(&timer) == 1);
  assert(timer.repeat == 50);
  /* New timeout should be based on current time + repeat */
  assert(timeout2 >= loop.time);
  
  /* Test again without repeat */
  timer_stop(&timer);
  timer.repeat = 0;
  assert(timer_again(&timer) == 0);
  assert(timer_is_active(&timer) == 0);
  
  event_loop_free(&loop);
  printf("✓ test_timer_again passed\n");
}

static void test_timer_heap_order(void) {
  struct event_loop loop;
  struct timer timer1, timer2, timer3;
  uint64_t key;
  void* data;
  
  event_loop_init(&loop);
  
  timer_init(&loop, &timer1);
  timer_init(&loop, &timer2);
  timer_init(&loop, &timer3);
  
  /* Start timers with different timeouts */
  timer_start(&timer1, timer_callback, 200, 0);
  timer_start(&timer2, timer_callback, 100, 0);
  timer_start(&timer3, timer_callback, 150, 0);
  
  /* Check heap order - minimum should be timer2 (100ms) */
  assert(!heap_empty(loop.timer_heap));
  assert(heap_extract_min(loop.timer_heap, &key, &data) == 0);
  assert(data == &timer2);
  assert(key == timer2.timeout);
  
  /* Next should be timer3 (150ms) */
  assert(heap_extract_min(loop.timer_heap, &key, &data) == 0);
  assert(data == &timer3);
  
  /* Last should be timer1 (200ms) */
  assert(heap_extract_min(loop.timer_heap, &key, &data) == 0);
  assert(data == &timer1);
  
  /* Cleanup */
  timer_stop(&timer1);
  timer_stop(&timer2);
  timer_stop(&timer3);
  
  event_loop_free(&loop);
  printf("✓ test_timer_heap_order passed\n");
}

int main(void) {
  printf("Running timer tests...\n\n");
  
  test_timer_init();
  test_timer_start_stop();
  test_timer_repeat();
  test_timer_due_in();
  test_timer_again();
  test_timer_heap_order();
  
  printf("\nAll timer tests passed!\n");
  return 0;
}

