#include <stdio.h>
#include <stdlib.h>
#include "../src/queue.h"

/*
 * Example: Using intrusive queue to manage a list of tasks.
 */

struct task {
  int id;
  char* description;
  struct queue q;
};

static void print_tasks(struct queue* head) {
  printf("Tasks:\n");
  
  if (queue_empty(head)) {
    printf("  (empty)\n");
    return;
  }
  
  struct queue* q;
  queue_foreach(q, head) {
    struct task* t = queue_data(q, struct task, q);
    printf("  Task %d: %s\n", t->id, t->description);
  }
}

int main(void) {
  struct queue task_queue;
  struct task task1, task2, task3;
  
  queue_init(&task_queue);
  
  task1.id = 1;
  task1.description = "Write code";
  queue_insert_tail(&task_queue, &task1.q);
  
  task2.id = 2;
  task2.description = "Write tests";
  queue_insert_tail(&task_queue, &task2.q);
  
  task3.id = 3;
  task3.description = "Write documentation";
  queue_insert_tail(&task_queue, &task3.q);
  
  printf("Initial tasks:\n");
  print_tasks(&task_queue);
  
  printf("\nProcessing first task:\n");
  struct queue* first = queue_head(&task_queue);
  struct task* t = queue_data(first, struct task, q);
  printf("  Processing: Task %d: %s\n", t->id, t->description);
  queue_remove(first);
  
  printf("\nRemaining tasks:\n");
  print_tasks(&task_queue);
  
  printf("\nAdding high-priority task at head:\n");
  struct task task4;
  task4.id = 4;
  task4.description = "Fix bug";
  queue_insert_head(&task_queue, &task4.q);
  print_tasks(&task_queue);
  
  return 0;
}

