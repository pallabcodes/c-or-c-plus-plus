#include "time.h"
#include <time.h>
#include <stdint.h>

#if defined(__APPLE__)
#include <mach/mach_time.h>
#include <mach/mach.h>
#elif defined(_WIN32)
#include <windows.h>
#else
#include <time.h>
#endif

uint64_t time_now_ns(void) {
#if defined(__APPLE__)
  /* macOS: Use mach_absolute_time for high-resolution monotonic time */
  static mach_timebase_info_data_t timebase = {0};
  if (timebase.denom == 0) {
    mach_timebase_info(&timebase);
  }
  uint64_t t = mach_absolute_time();
  return t * timebase.numer / timebase.denom;
#elif defined(_WIN32)
  /* Windows: Use QueryPerformanceCounter */
  LARGE_INTEGER counter, frequency;
  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&counter);
  return (uint64_t)(counter.QuadPart * 1000000000ULL / frequency.QuadPart);
#else
  /* Linux/Unix: Use clock_gettime with CLOCK_MONOTONIC */
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
    /* Fallback to CLOCK_REALTIME if CLOCK_MONOTONIC not available */
    clock_gettime(CLOCK_REALTIME, &ts);
  }
  return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
#endif
}

uint64_t time_now_ms(void) {
  return time_ns_to_ms(time_now_ns());
}

