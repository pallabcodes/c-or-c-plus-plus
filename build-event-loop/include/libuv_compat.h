/*
 * Compatibility header for using libuv files directly from Node.js source
 * 
 * This header provides convenient includes for libuv components that can be
 * used directly in build-event-loop instead of reimplementing them.
 * 
 * Usage:
 *   #include "libuv_compat.h"  // Then use libuv types/functions
 *   #include "queue.h"         // Direct include from libuv
 */

#ifndef LIBUV_COMPAT_H_
#define LIBUV_COMPAT_H_

/*
 * Option 1: Use libuv queue.h directly
 * 
 * Instead of: #include "queue.h" (local implementation)
 * Use:        #include "queue.h" (after setting up include paths)
 * 
 * Note: libuv uses uv__queue, while our implementation uses queue.
 * You may need to use uv__queue_* functions or create a compatibility layer.
 */

/*
 * Option 2: Include specific libuv headers
 * 
 * Uncomment what you need:
 */

// #include "queue.h"              // Intrusive queue from libuv
// #include "heap-inl.h"           // Heap implementation
// #include "timer.c"               // Timer management (if needed as header)
// #include "unix/internal.h"       // Internal libuv definitions

/*
 * Compatibility macros (if needed)
 * 
 * If you want to use libuv's uv__queue but prefer queue naming:
 */
#ifdef USE_LIBUV_QUEUE
  // Map libuv queue to our naming convention
  typedef struct uv__queue queue;
  #define queue_init uv__queue_init
  #define queue_empty uv__queue_empty
  #define queue_head uv__queue_head
  #define queue_insert_head uv__queue_insert_head
  #define queue_insert_tail uv__queue_insert_tail
  #define queue_remove uv__queue_remove
  #define queue_data uv__queue_data
  #define queue_foreach uv__queue_foreach
#endif

#endif /* LIBUV_COMPAT_H_ */

