#ifndef HEAP_H_
#define HEAP_H_

#include <stddef.h>
#include <stdint.h>

/*
 * Array-based binary min-heap implementation.
 * 
 * A min-heap is a complete binary tree where each parent node is less than
 * or equal to its children. This property ensures the minimum element is
 * always at the root.
 * 
 * Array-based implementation provides:
 * - Better cache locality than pointer-based trees
 * - Simpler implementation
 * - O(log n) insert and extract operations
 * - O(1) peek at minimum
 * 
 * Reference: node/deps/uv/src/heap-inl.h (libuv uses tree-based heap)
 * 
 * Note: For event loop timers, we'll use array-based heap for simplicity.
 * libuv uses tree-based heap for more complex operations.
 */

struct heap_node {
  uint64_t key;  // Priority/key value (e.g., timer expiry time)
  void* data;     // User data pointer
};

struct heap {
  struct heap_node* nodes;  // Array of heap nodes
  size_t capacity;          // Maximum capacity
  size_t size;              // Current size
};

/*
 * Initialize an empty heap.
 * 
 * @param h: Heap to initialize
 * @param capacity: Initial capacity (will grow if needed)
 * @return: 0 on success, -1 on error
 */
int heap_init(struct heap* h, size_t capacity);

/*
 * Free heap resources.
 * 
 * @param h: Heap to free
 */
void heap_free(struct heap* h);

/*
 * Check if heap is empty.
 * 
 * @param h: Heap to check
 * @return: 1 if empty, 0 otherwise
 */
static inline int heap_empty(const struct heap* h) {
  return h->size == 0;
}

/*
 * Get the minimum element without removing it.
 * 
 * @param h: Heap
 * @return: Pointer to minimum node, or NULL if empty
 */
static inline struct heap_node* heap_min(const struct heap* h) {
  if (heap_empty(h))
    return NULL;
  return &h->nodes[0];
}

/*
 * Insert a new element into the heap.
 * 
 * Time complexity: O(log n)
 * 
 * @param h: Heap
 * @param key: Priority/key value
 * @param data: User data pointer
 * @return: 0 on success, -1 on error
 */
int heap_insert(struct heap* h, uint64_t key, void* data);

/*
 * Extract and remove the minimum element.
 * 
 * Time complexity: O(log n)
 * 
 * @param h: Heap
 * @param key: Output parameter for extracted key
 * @param data: Output parameter for extracted data
 * @return: 0 on success, -1 if heap is empty
 */
int heap_extract_min(struct heap* h, uint64_t* key, void** data);

/*
 * Remove a specific element from the heap.
 * 
 * Time complexity: O(log n)
 * 
 * @param h: Heap
 * @param index: Index of element to remove
 * @return: 0 on success, -1 on error
 */
int heap_remove(struct heap* h, size_t index);

/*
 * Get the key at a specific index.
 * 
 * @param h: Heap
 * @param index: Index
 * @return: Key value, or 0 if invalid index
 */
static inline uint64_t heap_get_key(const struct heap* h, size_t index) {
  if (index >= h->size)
    return 0;
  return h->nodes[index].key;
}

/*
 * Get the size of the heap.
 * 
 * @param h: Heap
 * @return: Number of elements
 */
static inline size_t heap_size(const struct heap* h) {
  return h->size;
}

#endif /* HEAP_H_ */

