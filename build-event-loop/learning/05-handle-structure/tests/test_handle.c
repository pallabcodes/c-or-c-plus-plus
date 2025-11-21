#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/handle.h"
#include "../04-loop-structure/src/event_loop.h"

static void test_handle_init(void) {
  struct event_loop loop;
  struct handle handle;
  
  event_loop_init(&loop);
  handle_init(&handle, &loop, HANDLE_TYPE_IDLE);
  
  assert(handle.loop == &loop);
  assert(handle.type == HANDLE_TYPE_IDLE);
  assert(handle.data == NULL);
  assert(handle.close_cb == NULL);
  assert(handle.flags == 0);
  assert(handle.u.fd == -1);
  assert(queue_empty(&handle.handle_queue));
  
  event_loop_free(&loop);
  printf("✓ test_handle_init passed\n");
}

static void test_handle_active(void) {
  struct event_loop loop;
  struct handle handle;
  
  event_loop_init(&loop);
  handle_init(&handle, &loop, HANDLE_TYPE_TIMER);
  
  assert(handle_is_active(&handle) == 0);
  
  handle_set_active(&handle);
  assert(handle_is_active(&handle) == 1);
  
  handle_set_inactive(&handle);
  assert(handle_is_active(&handle) == 0);
  
  event_loop_free(&loop);
  printf("✓ test_handle_active passed\n");
}

static int close_called = 0;

static void close_cb(struct handle* h) {
  (void)h;  /* Unused parameter */
  close_called = 1;
}

static void test_handle_closing(void) {
  struct event_loop loop;
  struct handle handle;
  
  close_called = 0;
  
  event_loop_init(&loop);
  handle_init(&handle, &loop, HANDLE_TYPE_CHECK);
  
  assert(handle_is_closing(&handle) == 0);
  
  handle_start_closing(&handle, close_cb);
  assert(handle_is_closing(&handle) == 1);
  assert(handle.close_cb == close_cb);
  
  event_loop_free(&loop);
  printf("✓ test_handle_closing passed\n");
}

static void test_handle_data(void) {
  struct event_loop loop;
  struct handle handle;
  int user_data = 42;
  
  event_loop_init(&loop);
  handle_init(&handle, &loop, HANDLE_TYPE_IO);
  
  assert(handle_get_data(&handle) == NULL);
  
  handle_set_data(&handle, &user_data);
  assert(handle_get_data(&handle) == &user_data);
  assert(handle.data == &user_data);
  
  event_loop_free(&loop);
  printf("✓ test_handle_data passed\n");
}

static void test_handle_type(void) {
  struct event_loop loop;
  struct handle handle;
  
  event_loop_init(&loop);
  
  handle_init(&handle, &loop, HANDLE_TYPE_IDLE);
  assert(handle_get_type(&handle) == HANDLE_TYPE_IDLE);
  assert(strcmp(handle_type_name(HANDLE_TYPE_IDLE), "IDLE") == 0);
  
  handle_init(&handle, &loop, HANDLE_TYPE_TIMER);
  assert(handle_get_type(&handle) == HANDLE_TYPE_TIMER);
  assert(strcmp(handle_type_name(HANDLE_TYPE_TIMER), "TIMER") == 0);
  
  event_loop_free(&loop);
  printf("✓ test_handle_type passed\n");
}

static void test_handle_loop(void) {
  struct event_loop loop1, loop2;
  struct handle handle;
  
  event_loop_init(&loop1);
  event_loop_init(&loop2);
  
  handle_init(&handle, &loop1, HANDLE_TYPE_PREPARE);
  assert(handle_get_loop(&handle) == &loop1);
  
  handle.loop = &loop2;
  assert(handle_get_loop(&handle) == &loop2);
  
  event_loop_free(&loop1);
  event_loop_free(&loop2);
  printf("✓ test_handle_loop passed\n");
}

int main(void) {
  printf("Running handle tests...\n\n");
  
  test_handle_init();
  test_handle_active();
  test_handle_closing();
  test_handle_data();
  test_handle_type();
  test_handle_loop();
  
  printf("\nAll handle tests passed!\n");
  return 0;
}

