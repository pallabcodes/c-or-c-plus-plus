#include <stdio.h>
#include <unistd.h>
#include "../src/time.h"

int main(void) {
  printf("Time Management Example\n\n");
  
  /* Get current time */
  uint64_t start = time_now_ms();
  printf("Start time: %lu ms\n", start);
  
  /* Sleep for 100ms */
  printf("Sleeping for 100ms...\n");
  usleep(100000);
  
  uint64_t end = time_now_ms();
  printf("End time: %lu ms\n", end);
  
  uint64_t elapsed = time_diff_ms(end, start);
  printf("Elapsed: %lu ms\n\n", elapsed);
  
  /* Timer example */
  printf("Timer example:\n");
  uint64_t timer_expiry = time_add_ms(time_now_ms(), 50);
  printf("Timer set to expire in 50ms\n");
  
  while (!time_expired(timer_expiry, time_now_ms())) {
    usleep(10000);  // Sleep 10ms
  }
  
  uint64_t actual_elapsed = time_diff_ms(time_now_ms(), timer_expiry + 50);
  printf("Timer expired! (target: 50ms, actual: ~%lu ms)\n", 
         actual_elapsed > 50 ? actual_elapsed - 50 : 0);
  
  return 0;
}

