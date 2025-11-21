#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../src/time.h"

static void test_time_now(void) {
  uint64_t t1 = time_now_ms();
  uint64_t t2 = time_now_ms();
  
  assert(t2 >= t1);
  printf("PASS: test_time_now\n");
}

static void test_time_monotonic(void) {
  uint64_t t1 = time_now_ms();
  usleep(10000);  // Sleep 10ms
  uint64_t t2 = time_now_ms();
  
  uint64_t diff = time_diff_ms(t2, t1);
  assert(diff >= 5);   // At least 5ms should have passed
  assert(diff <= 50);  // But not more than 50ms (account for scheduling)
  
  printf("PASS: test_time_monotonic (diff: %lu ms)\n", diff);
}

static void test_time_conversion(void) {
  uint64_t ns = 1234567890ULL;
  uint64_t ms = time_ns_to_ms(ns);
  uint64_t ns2 = time_ms_to_ns(ms);
  
  assert(ms == 1234);
  assert(ns2 == 1234000000ULL);
  
  printf("PASS: test_time_conversion\n");
}

static void test_time_arithmetic(void) {
  uint64_t base = 1000;
  uint64_t added = time_add_ms(base, 500);
  
  assert(added == 1500);
  assert(time_diff_ms(added, base) == 500);
  
  printf("PASS: test_time_arithmetic\n");
}

static void test_time_comparison(void) {
  uint64_t t1 = 1000;
  uint64_t t2 = 2000;
  
  assert(time_before(t1, t2));
  assert(time_after(t2, t1));
  assert(!time_before(t2, t1));
  assert(!time_after(t1, t2));
  
  printf("PASS: test_time_comparison\n");
}

static void test_time_expired(void) {
  uint64_t now = time_now_ms();
  uint64_t past = now - 1000;
  uint64_t future = now + 1000;
  
  assert(time_expired(past, now));
  assert(!time_expired(future, now));
  assert(time_expired(now, now));  // Exactly now is considered expired
  
  printf("PASS: test_time_expired\n");
}

static void test_time_precision(void) {
  uint64_t ns1 = time_now_ns();
  uint64_t ns2 = time_now_ns();
  
  /* Should have nanosecond precision */
  assert(ns2 >= ns1);
  
  uint64_t ms1 = time_now_ms();
  uint64_t ms2 = time_now_ms();
  
  /* Milliseconds should be consistent with nanoseconds */
  uint64_t ns_ms1 = time_ms_to_ns(ms1);
  uint64_t ns_ms2 = time_ms_to_ns(ms2);
  
  assert(ns_ms1 <= ns1 + 1000000);  // Within 1ms
  assert(ns_ms2 <= ns2 + 1000000);
  
  printf("PASS: test_time_precision\n");
}

int main(void) {
  printf("Running time management tests...\n\n");
  
  test_time_now();
  test_time_monotonic();
  test_time_conversion();
  test_time_arithmetic();
  test_time_comparison();
  test_time_expired();
  test_time_precision();
  
  printf("\nAll tests passed!\n");
  return 0;
}

