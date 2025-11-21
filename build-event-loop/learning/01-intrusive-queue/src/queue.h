#ifndef QUEUE_H_
#define QUEUE_H_

#include <stddef.h>

/*
 * Intrusive doubly-linked list queue structure.
 * 
 * This is an intrusive data structure, meaning the queue node is embedded
 * directly in the containing structure, eliminating the need for separate
 * node allocations. This provides O(1) insertion/removal and better cache
 * locality.
 * 
 * Reference: node/deps/uv/src/queue.h
 */

struct queue {
  struct queue* next;
  struct queue* prev;
};

/*
 * Get the containing structure from a queue pointer.
 * 
 * This macro uses offsetof to calculate the address of the containing
 * structure given a pointer to the queue member.
 * 
 * @param pointer: Pointer to the queue member
 * @param type: Type of the containing structure
 * @param field: Name of the queue field in the containing structure
 * 
 * Example:
 *   struct my_struct {
 *     int data;
 *     struct queue q;
 *   };
 *   struct queue* q_ptr = &my_struct_instance.q;
 *   struct my_struct* container = queue_data(q_ptr, struct my_struct, q);
 */
#define queue_data(pointer, type, field) \
  ((type*) ((char*) (pointer) - offsetof(type, field)))

/*
 * Iterate over all queue entries.
 * 
 * @param q: Current queue pointer (iterator)
 * @param h: Head of the queue
 * 
 * Example:
 *   struct queue* q;
 *   queue_foreach(q, &head) {
 *     // Process q
 *   }
 */
#define queue_foreach(q, h) \
  for ((q) = (h)->next; (q) != (h); (q) = (q)->next)

/*
 * Initialize an empty queue.
 * 
 * An empty queue points to itself (circular structure).
 * 
 * @param q: Queue to initialize
 */
static inline void queue_init(struct queue* q) {
  q->next = q;
  q->prev = q;
}

/*
 * Check if queue is empty.
 * 
 * @param q: Queue to check
 * @return: 1 if empty, 0 otherwise
 */
static inline int queue_empty(const struct queue* q) {
  return q == q->next;
}

/*
 * Get the head (first element) of the queue.
 * 
 * @param q: Queue head pointer
 * @return: Pointer to first element, or q if empty
 */
static inline struct queue* queue_head(const struct queue* q) {
  return q->next;
}

/*
 * Get the next element in the queue.
 * 
 * @param q: Current queue element
 * @return: Pointer to next element
 */
static inline struct queue* queue_next(const struct queue* q) {
  return q->next;
}

/*
 * Get the previous element in the queue.
 * 
 * @param q: Current queue element
 * @return: Pointer to previous element
 */
static inline struct queue* queue_prev(const struct queue* q) {
  return q->prev;
}

/*
 * Insert element at the head of the queue.
 * 
 * @param h: Queue head pointer
 * @param q: Element to insert
 */
static inline void queue_insert_head(struct queue* h, struct queue* q) {
  q->next = h->next;
  q->prev = h;
  q->next->prev = q;
  h->next = q;
}

/*
 * Insert element at the tail of the queue.
 * 
 * @param h: Queue head pointer
 * @param q: Element to insert
 */
static inline void queue_insert_tail(struct queue* h, struct queue* q) {
  q->next = h;
  q->prev = h->prev;
  q->prev->next = q;
  h->prev = q;
}

/*
 * Remove an element from the queue.
 * 
 * This does not free the element, just removes it from the queue.
 * 
 * @param q: Element to remove
 */
static inline void queue_remove(struct queue* q) {
  q->prev->next = q->next;
  q->next->prev = q->prev;
}

/*
 * Add all elements from queue n to queue h.
 * 
 * This merges two queues by appending all elements from n to h.
 * After this operation, queue n will be empty.
 * 
 * @param h: Destination queue head
 * @param n: Source queue head (will be emptied)
 */
static inline void queue_add(struct queue* h, struct queue* n) {
  if (queue_empty(n))
    return;
  
  h->prev->next = n->next;
  n->next->prev = h->prev;
  h->prev = n->prev;
  h->prev->next = h;
  
  queue_init(n);
}

/*
 * Split queue h at element q, moving elements after q to queue n.
 * 
 * @param h: Original queue head
 * @param q: Element where to split
 * @param n: New queue head for elements after q
 */
static inline void queue_split(struct queue* h,
                               struct queue* q,
                               struct queue* n) {
  n->prev = h->prev;
  n->prev->next = n;
  n->next = q;
  h->prev = q->prev;
  h->prev->next = h;
  q->prev = n;
}

/*
 * Move all elements from queue h to queue n.
 * 
 * @param h: Source queue head (will be emptied)
 * @param n: Destination queue head
 */
static inline void queue_move(struct queue* h, struct queue* n) {
  if (queue_empty(h))
    queue_init(n);
  else
    queue_split(h, h->next, n);
}

#endif /* QUEUE_H_ */