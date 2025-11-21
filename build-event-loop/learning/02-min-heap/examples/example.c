#include <stdio.h>
#include <stdlib.h>
#include "../src/heap.h"

/*
 * Example: Using min-heap as a priority queue for tasks.
 */

struct task {
  int id;
  char* description;
  uint64_t priority;  // Lower number = higher priority
};

int main(void) {
  struct heap h;
  struct task tasks[] = {
    {1, "Low priority task", 100},
    {2, "High priority task", 10},
    {3, "Medium priority task", 50},
    {4, "Urgent task", 5},
  };
  
  if (heap_init(&h, 10) != 0) {
    fprintf(stderr, "Failed to initialize heap\n");
    return 1;
  }
  
  printf("Inserting tasks into priority queue:\n");
  for (int i = 0; i < 4; i++) {
    printf("  Task %d: %s (priority: %lu)\n", 
           tasks[i].id, tasks[i].description, tasks[i].priority);
    heap_insert(&h, tasks[i].priority, &tasks[i]);
  }
  
  printf("\nProcessing tasks in priority order:\n");
  uint64_t priority;
  void* data;
  while (!heap_empty(&h)) {
    if (heap_extract_min(&h, &priority, &data) == 0) {
      struct task* t = (struct task*)data;
      printf("  Processing: Task %d: %s (priority: %lu)\n",
             t->id, t->description, priority);
    }
  }
  
  heap_free(&h);
  return 0;
}

