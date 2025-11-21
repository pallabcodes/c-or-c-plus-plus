#include <stdio.h>
#include "../src/timer_heap.h"
#include "../04-loop-structure/src/event_loop.h"
#include "../07-timer-structure/src/timer.h"

static void timer_callback(struct timer* timer) {
  (void)timer;
  printf("Timer fired!\n");
}

int main(void) {
  struct event_loop loop;
  struct timer timer1, timer2, timer3;
  int timeout;
  struct timer* min_timer;
  
  /* Initialize event loop */
  event_loop_init(&loop);
  
  /* Initialize timers */
  timer_init(&loop, &timer1);
  timer_init(&loop, &timer2);
  timer_init(&loop, &timer3);
  
  printf("Timer Heap Operations Example\n");
  printf("=============================\n\n");
  
  /* Check empty heap */
  printf("Empty heap:\n");
  printf("  Is empty: %d\n", timer_heap_empty(&loop));
  printf("  Count: %zu\n", timer_heap_count(&loop));
  printf("  Next timeout: %d\n", timer_heap_next_timeout(&loop));
  
  /* Add timers */
  printf("\nAdding timers:\n");
  timer_start(&timer1, timer_callback, 300, 0);
  printf("  Timer1: 300ms\n");
  
  timer_start(&timer2, timer_callback, 100, 0);
  printf("  Timer2: 100ms\n");
  
  timer_start(&timer3, timer_callback, 200, 0);
  printf("  Timer3: 200ms\n");
  
  /* Check heap state */
  printf("\nHeap state:\n");
  printf("  Is empty: %d\n", timer_heap_empty(&loop));
  printf("  Count: %zu\n", timer_heap_count(&loop));
  
  /* Get minimum timer */
  min_timer = timer_heap_min(&loop);
  if (min_timer != NULL) {
    printf("  Minimum timer: timeout=%llu ms\n", 
           (unsigned long long)min_timer->timeout);
  }
  
  /* Get next timeout */
  timeout = timer_heap_next_timeout(&loop);
  printf("  Next timeout: %d ms\n", timeout);
  
  /* Remove timer2 and check again */
  printf("\nRemoving timer2 (100ms):\n");
  timer_stop(&timer2);
  
  min_timer = timer_heap_min(&loop);
  if (min_timer != NULL) {
    printf("  Minimum timer: timeout=%llu ms\n", 
           (unsigned long long)min_timer->timeout);
  }
  
  timeout = timer_heap_next_timeout(&loop);
  printf("  Next timeout: %d ms\n", timeout);
  
  /* Cleanup */
  timer_stop(&timer1);
  timer_stop(&timer3);
  event_loop_free(&loop);
  
  printf("\nExample complete!\n");
  return 0;
}

