#include <stdio.h>
#include "../src/timer.h"
#include "../04-loop-structure/src/event_loop.h"

/*
 * Example: Using timer structure.
 * 
 * This demonstrates how timers work in the event loop.
 */

static int timer1_count = 0;
static int timer2_count = 0;

static void timer1_callback(struct timer* timer) {
  (void)timer;
  timer1_count++;
  printf("Timer 1 fired! Count: %d\n", timer1_count);
}

static void timer2_callback(struct timer* timer) {
  (void)timer;
  timer2_count++;
  printf("Timer 2 fired! Count: %d\n", timer2_count);
}

int main(void) {
  struct event_loop loop;
  struct timer timer1, timer2;
  
  /* Initialize event loop */
  event_loop_init(&loop);
  
  /* Initialize timers */
  timer_init(&loop, &timer1);
  timer_init(&loop, &timer2);
  
  printf("Timer Example\n");
  printf("=============\n\n");
  
  /* Start timer1 - one-shot timer for 100ms */
  printf("Starting timer1: one-shot, 100ms\n");
  timer_start(&timer1, timer1_callback, 100, 0);
  printf("  Timer1 timeout: %llu ms\n", (unsigned long long)timer1.timeout);
  printf("  Timer1 due in: %llu ms\n", (unsigned long long)timer_get_due_in(&timer1));
  printf("  Timer1 active: %d\n", timer_is_active(&timer1));
  
  /* Start timer2 - repeating timer every 50ms */
  printf("\nStarting timer2: repeating, 50ms interval\n");
  timer_start(&timer2, timer2_callback, 50, 50);
  printf("  Timer2 timeout: %llu ms\n", (unsigned long long)timer2.timeout);
  printf("  Timer2 repeat: %llu ms\n", (unsigned long long)timer_get_repeat(&timer2));
  printf("  Timer2 due in: %llu ms\n", (unsigned long long)timer_get_due_in(&timer2));
  printf("  Timer2 active: %d\n", timer_is_active(&timer2));
  
  /* Change timer2 repeat interval */
  printf("\nChanging timer2 repeat interval to 75ms\n");
  timer_set_repeat(&timer2, 75);
  printf("  Timer2 repeat: %llu ms\n", (unsigned long long)timer_get_repeat(&timer2));
  
  /* Stop timer1 */
  printf("\nStopping timer1\n");
  timer_stop(&timer1);
  printf("  Timer1 active: %d\n", timer_is_active(&timer1));
  
  /* Restart timer2 with again() */
  printf("\nRestarting timer2 with timer_again()\n");
  timer_stop(&timer2);
  timer_again(&timer2);
  printf("  Timer2 active: %d\n", timer_is_active(&timer2));
  printf("  Timer2 timeout: %llu ms\n", (unsigned long long)timer2.timeout);
  
  /* Cleanup */
  timer_stop(&timer1);
  timer_stop(&timer2);
  event_loop_free(&loop);
  
  printf("\nExample complete!\n");
  return 0;
}

