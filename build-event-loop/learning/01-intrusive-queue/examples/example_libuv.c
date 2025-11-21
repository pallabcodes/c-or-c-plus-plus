#include <stdio.h>
#include <stdlib.h>
#include "../src/queue_libuv.h"

/*
 * Example: Using libuv's queue.h directly.
 * 
 * This demonstrates using libuv's production-grade queue implementation
 * instead of our simplified version.
 */

struct task {
  int id;
  char* description;
  struct uv__queue q;  // Use libuv's queue structure
};

static void print_tasks(struct uv__queue* head) {
  printf("Tasks (using libuv's queue):\n");
  
  if (uv__queue_empty(head)) {
    printf("  (empty)\n");
    return;
  }
  
  struct uv__queue* q;
  uv__queue_foreach(q, head) {
    struct task* t = uv__queue_data(q, struct task, q);
    printf("  Task %d: %s\n", t->id, t->description);
  }
}

int main(void) {
  struct uv__queue task_queue;
  struct task task1, task2, task3;
  
  uv__queue_init(&task_queue);
  
  task1.id = 1;
  task1.description = "Write code";
  uv__queue_insert_tail(&task_queue, &task1.q);
  
  task2.id = 2;
  task2.description = "Write tests";
  uv__queue_insert_tail(&task_queue, &task2.q);
  
  task3.id = 3;
  task3.description = "Write documentation";
  uv__queue_insert_tail(&task_queue, &task3.q);
  
  printf("Initial tasks:\n");
  print_tasks(&task_queue);
  
  printf("\nProcessing first task:\n");
  struct uv__queue* first = uv__queue_head(&task_queue);
  struct task* t = uv__queue_data(first, struct task, q);
  printf("  Processing: Task %d: %s\n", t->id, t->description);
  uv__queue_remove(first);
  
  printf("\nRemaining tasks:\n");
  print_tasks(&task_queue);
  
  printf("\nAdding high-priority task at head:\n");
  struct task task4;
  task4.id = 4;
  task4.description = "Fix bug";
  uv__queue_insert_head(&task_queue, &task4.q);
  print_tasks(&task_queue);
  
  printf("\nNote: This example uses libuv's queue.h directly from:\n");
  printf("  node/deps/uv/src/queue.h\n");
  printf("This is the actual production code used by Node.js!\n");
  
  return 0;
}
