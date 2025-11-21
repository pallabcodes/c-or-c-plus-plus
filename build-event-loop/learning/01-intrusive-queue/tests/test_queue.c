#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "../src/queue.h"

/*
 * Test structure containing a queue node.
 */
struct test_item {
  int value;
  struct queue q;
};

/*
 * Test queue initialization.
 */
static void test_queue_init(void) {
  struct queue head;
  queue_init(&head);
  
  assert(queue_empty(&head));
  assert(queue_head(&head) == &head);
  assert(queue_next(&head) == &head);
  assert(queue_prev(&head) == &head);
  
  printf("PASS: test_queue_init\n");
}

/*
 * Test inserting at head.
 */
static void test_queue_insert_head(void) {
  struct queue head;
  struct test_item item1, item2, item3;
  
  queue_init(&head);
  item1.value = 1;
  item2.value = 2;
  item3.value = 3;
  
  queue_insert_head(&head, &item1.q);
  assert(!queue_empty(&head));
  assert(queue_head(&head) == &item1.q);
  
  queue_insert_head(&head, &item2.q);
  assert(queue_head(&head) == &item2.q);
  assert(queue_next(&item2.q) == &item1.q);
  
  queue_insert_head(&head, &item3.q);
  assert(queue_head(&head) == &item3.q);
  
  printf("PASS: test_queue_insert_head\n");
}

/*
 * Test inserting at tail.
 */
static void test_queue_insert_tail(void) {
  struct queue head;
  struct test_item item1, item2, item3;
  
  queue_init(&head);
  item1.value = 1;
  item2.value = 2;
  item3.value = 3;
  
  queue_insert_tail(&head, &item1.q);
  assert(queue_head(&head) == &item1.q);
  
  queue_insert_tail(&head, &item2.q);
  assert(queue_head(&head) == &item1.q);
  assert(queue_next(&item1.q) == &item2.q);
  
  queue_insert_tail(&head, &item3.q);
  assert(queue_next(&item2.q) == &item3.q);
  
  printf("PASS: test_queue_insert_tail\n");
}

/*
 * Test removing elements.
 */
static void test_queue_remove(void) {
  struct queue head;
  struct test_item item1, item2, item3;
  
  queue_init(&head);
  item1.value = 1;
  item2.value = 2;
  item3.value = 3;
  
  queue_insert_tail(&head, &item1.q);
  queue_insert_tail(&head, &item2.q);
  queue_insert_tail(&head, &item3.q);
  
  queue_remove(&item2.q);
  assert(queue_next(&item1.q) == &item3.q);
  assert(queue_prev(&item3.q) == &item1.q);
  
  queue_remove(&item1.q);
  assert(queue_head(&head) == &item3.q);
  
  queue_remove(&item3.q);
  assert(queue_empty(&head));
  
  printf("PASS: test_queue_remove\n");
}

/*
 * Test queue_foreach macro.
 */
static void test_queue_foreach(void) {
  struct queue head;
  struct test_item items[5];
  int i;
  int sum = 0;
  
  queue_init(&head);
  
  for (i = 0; i < 5; i++) {
    items[i].value = i + 1;
    queue_insert_tail(&head, &items[i].q);
  }
  
  struct queue* q;
  queue_foreach(q, &head) {
    struct test_item* item = queue_data(q, struct test_item, q);
    sum += item->value;
  }
  
  assert(sum == 15);
  printf("PASS: test_queue_foreach\n");
}

/*
 * Test queue_add (merging queues).
 */
static void test_queue_add(void) {
  struct queue head1, head2;
  struct test_item items[6];
  int i;
  
  queue_init(&head1);
  queue_init(&head2);
  
  for (i = 0; i < 3; i++) {
    items[i].value = i + 1;
    queue_insert_tail(&head1, &items[i].q);
  }
  
  for (i = 3; i < 6; i++) {
    items[i].value = i + 1;
    queue_insert_tail(&head2, &items[i].q);
  }
  
  queue_add(&head1, &head2);
  assert(queue_empty(&head2));
  
  int count = 0;
  struct queue* q;
  queue_foreach(q, &head1) {
    count++;
  }
  assert(count == 6);
  
  printf("PASS: test_queue_add\n");
}

/*
 * Test queue_move.
 */
static void test_queue_move(void) {
  struct queue head1, head2;
  struct test_item items[3];
  int i;
  
  queue_init(&head1);
  queue_init(&head2);
  
  for (i = 0; i < 3; i++) {
    items[i].value = i + 1;
    queue_insert_tail(&head1, &items[i].q);
  }
  
  queue_move(&head1, &head2);
  assert(queue_empty(&head1));
  assert(!queue_empty(&head2));
  
  int count = 0;
  struct queue* q;
  queue_foreach(q, &head2) {
    count++;
  }
  assert(count == 3);
  
  printf("PASS: test_queue_move\n");
}

/*
 * Test queue_split.
 */
static void test_queue_split(void) {
  struct queue head1, head2;
  struct test_item items[5];
  int i;
  
  queue_init(&head1);
  queue_init(&head2);
  
  for (i = 0; i < 5; i++) {
    items[i].value = i + 1;
    queue_insert_tail(&head1, &items[i].q);
  }
  
  queue_split(&head1, &items[2].q, &head2);
  
  int count1 = 0, count2 = 0;
  struct queue* q;
  
  queue_foreach(q, &head1) {
    count1++;
  }
  
  queue_foreach(q, &head2) {
    count2++;
  }
  
  assert(count1 == 2);
  assert(count2 == 3);
  
  printf("PASS: test_queue_split\n");
}

int main(void) {
  printf("Running queue tests...\n\n");
  
  test_queue_init();
  test_queue_insert_head();
  test_queue_insert_tail();
  test_queue_remove();
  test_queue_foreach();
  test_queue_add();
  test_queue_move();
  test_queue_split();
  
  printf("\nAll tests passed!\n");
  return 0;
}

