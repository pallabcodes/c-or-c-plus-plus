#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "../src/heap.h"

static void test_heap_init(void) {
  struct heap h;
  
  assert(heap_init(&h, 10) == 0);
  assert(heap_empty(&h));
  assert(heap_min(&h) == NULL);
  assert(heap_size(&h) == 0);
  
  heap_free(&h);
  printf("PASS: test_heap_init\n");
}

static void test_heap_insert(void) {
  struct heap h;
  int values[] = {5, 2, 8, 1, 9, 3};
  int i;
  
  assert(heap_init(&h, 10) == 0);
  
  for (i = 0; i < 6; i++) {
    assert(heap_insert(&h, values[i], &values[i]) == 0);
  }
  
  assert(heap_size(&h) == 6);
  assert(heap_min(&h) != NULL);
  assert(heap_min(&h)->key == 1);  // Minimum should be 1
  
  heap_free(&h);
  printf("PASS: test_heap_insert\n");
}

static void test_heap_extract_min(void) {
  struct heap h;
  int values[] = {5, 2, 8, 1, 9, 3};
  int i;
  uint64_t key;
  void* data;
  
  assert(heap_init(&h, 10) == 0);
  
  for (i = 0; i < 6; i++) {
    assert(heap_insert(&h, values[i], &values[i]) == 0);
  }
  
  /* Extract elements in sorted order */
  int expected[] = {1, 2, 3, 5, 8, 9};
  for (i = 0; i < 6; i++) {
    assert(heap_extract_min(&h, &key, &data) == 0);
    assert(key == expected[i]);
    assert(data != NULL);  // Data pointer should be valid
  }
  
  assert(heap_empty(&h));
  assert(heap_extract_min(&h, &key, &data) == -1);
  
  heap_free(&h);
  printf("PASS: test_heap_extract_min\n");
}

static void test_heap_remove(void) {
  struct heap h;
  int values[] = {5, 2, 8, 1, 9, 3};
  int i;
  
  assert(heap_init(&h, 10) == 0);
  
  for (i = 0; i < 6; i++) {
    assert(heap_insert(&h, values[i], &values[i]) == 0);
  }
  
  /* Remove element at index 2 */
  assert(heap_remove(&h, 2) == 0);
  assert(heap_size(&h) == 5);
  
  /* Minimum should still be 1 */
  assert(heap_min(&h)->key == 1);
  
  heap_free(&h);
  printf("PASS: test_heap_remove\n");
}

static void test_heap_growth(void) {
  struct heap h;
  int i;
  
  assert(heap_init(&h, 2) == 0);
  
  /* Insert more than initial capacity */
  for (i = 0; i < 100; i++) {
    assert(heap_insert(&h, 100 - i, NULL) == 0);
  }
  
  assert(heap_size(&h) == 100);
  assert(heap_min(&h)->key == 1);  // Minimum should be 1
  
  heap_free(&h);
  printf("PASS: test_heap_growth\n");
}

static void test_heap_duplicate_keys(void) {
  struct heap h;
  int values[] = {5, 5, 5, 2, 2};
  int i;
  uint64_t key;
  
  assert(heap_init(&h, 10) == 0);
  
  for (i = 0; i < 5; i++) {
    assert(heap_insert(&h, values[i], &values[i]) == 0);
  }
  
  /* Extract should work with duplicates */
  assert(heap_extract_min(&h, &key, NULL) == 0);
  assert(key == 2);
  
  assert(heap_extract_min(&h, &key, NULL) == 0);
  assert(key == 2);
  
  assert(heap_extract_min(&h, &key, NULL) == 0);
  assert(key == 5);
  
  heap_free(&h);
  printf("PASS: test_heap_duplicate_keys\n");
}

int main(void) {
  printf("Running heap tests...\n\n");
  
  test_heap_init();
  test_heap_insert();
  test_heap_extract_min();
  test_heap_remove();
  test_heap_growth();
  test_heap_duplicate_keys();
  
  printf("\nAll tests passed!\n");
  return 0;
}

