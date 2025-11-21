#ifndef QUEUE_LIBUV_H_
#define QUEUE_LIBUV_H_

/*
 * Wrapper for libuv's queue.h - Direct import from Node.js/libuv.
 * 
 * This allows us to use libuv's production-grade queue implementation
 * directly while learning from it.
 * 
 * Source: node/deps/uv/src/queue.h
 *         node/deps/uv/include/uv.h (struct definition)
 * 
 * Usage:
 *   #include "queue_libuv.h"
 *   struct uv__queue head;
 *   uv__queue_init(&head);
 */

/* Define the struct (from uv.h line 64-67) */
struct uv__queue {
  struct uv__queue* next;
  struct uv__queue* prev;
};

/* Include the queue implementation */
#include "../../../../node/deps/uv/src/queue.h"

/*
 * Convenience macros to use libuv's queue with our naming.
 * This allows us to use libuv's implementation while maintaining
 * consistency with our codebase.
 */

#define queue_init uv__queue_init
#define queue_empty uv__queue_empty
#define queue_head uv__queue_head
#define queue_next uv__queue_next
#define queue_insert_head uv__queue_insert_head
#define queue_insert_tail uv__queue_insert_tail
#define queue_remove uv__queue_remove
#define queue_add uv__queue_add
#define queue_split uv__queue_split
#define queue_move uv__queue_move

/* Use libuv's queue structure */
#define queue uv__queue

/* Use libuv's queue_data macro */
#define queue_data uv__queue_data

/* Use libuv's queue_foreach macro */
#define queue_foreach uv__queue_foreach

#endif /* QUEUE_LIBUV_H_ */
