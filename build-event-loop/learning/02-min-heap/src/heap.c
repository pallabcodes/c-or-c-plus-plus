#include "heap.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
 * Helper: Get parent index.
 */
static inline size_t parent(size_t i) {
  return (i - 1) / 2;
}

/*
 * Helper: Get left child index.
 */
static inline size_t left(size_t i) {
  return 2 * i + 1;
}

/*
 * Helper: Get right child index.
 */
static inline size_t right(size_t i) {
  return 2 * i + 2;
}

/*
 * Helper: Swap two heap nodes.
 */
static void swap_nodes(struct heap_node* a, struct heap_node* b) {
  struct heap_node temp = *a;
  *a = *b;
  *b = temp;
}

/*
 * Helper: Bubble up (heapify up) to maintain heap property.
 * 
 * After inserting a new element, we need to move it up the tree
 * until the heap property is restored (parent <= children).
 */
static void heapify_up(struct heap* h, size_t index) {
  while (index > 0) {
    size_t p = parent(index);
    if (h->nodes[p].key <= h->nodes[index].key)
      break;
    
    swap_nodes(&h->nodes[p], &h->nodes[index]);
    index = p;
  }
}

/*
 * Helper: Bubble down (heapify down) to maintain heap property.
 * 
 * After removing the root or replacing an element, we need to move
 * it down the tree until the heap property is restored.
 */
static void heapify_down(struct heap* h, size_t index) {
  while (1) {
    size_t smallest = index;
    size_t l = left(index);
    size_t r = right(index);
    
    if (l < h->size && h->nodes[l].key < h->nodes[smallest].key)
      smallest = l;
    
    if (r < h->size && h->nodes[r].key < h->nodes[smallest].key)
      smallest = r;
    
    if (smallest == index)
      break;
    
    swap_nodes(&h->nodes[index], &h->nodes[smallest]);
    index = smallest;
  }
}

int heap_init(struct heap* h, size_t capacity) {
  if (capacity == 0)
    capacity = 16;  // Default capacity
  
  h->nodes = malloc(sizeof(struct heap_node) * capacity);
  if (h->nodes == NULL)
    return -1;
  
  h->capacity = capacity;
  h->size = 0;
  return 0;
}

void heap_free(struct heap* h) {
  if (h->nodes != NULL) {
    free(h->nodes);
    h->nodes = NULL;
  }
  h->capacity = 0;
  h->size = 0;
}

int heap_insert(struct heap* h, uint64_t key, void* data) {
  /* Grow heap if needed */
  if (h->size >= h->capacity) {
    size_t new_capacity = h->capacity * 2;
    struct heap_node* new_nodes = realloc(h->nodes, 
                                          sizeof(struct heap_node) * new_capacity);
    if (new_nodes == NULL)
      return -1;
    
    h->nodes = new_nodes;
    h->capacity = new_capacity;
  }
  
  /* Insert at the end */
  size_t index = h->size;
  h->nodes[index].key = key;
  h->nodes[index].data = data;
  h->size++;
  
  /* Bubble up to maintain heap property */
  heapify_up(h, index);
  
  return 0;
}

int heap_extract_min(struct heap* h, uint64_t* key, void** data) {
  if (heap_empty(h))
    return -1;
  
  /* Extract root (minimum) */
  if (key != NULL)
    *key = h->nodes[0].key;
  if (data != NULL)
    *data = h->nodes[0].data;
  
  /* Move last element to root */
  h->size--;
  if (h->size > 0) {
    h->nodes[0] = h->nodes[h->size];
    heapify_down(h, 0);
  }
  
  return 0;
}

int heap_remove(struct heap* h, size_t index) {
  if (index >= h->size)
    return -1;
  
  /* Move last element to this position */
  h->size--;
  if (index < h->size) {
    h->nodes[index] = h->nodes[h->size];
    
    /* Restore heap property */
    if (index > 0 && h->nodes[parent(index)].key > h->nodes[index].key) {
      heapify_up(h, index);
    } else {
      heapify_down(h, index);
    }
  }
  
  return 0;
}

