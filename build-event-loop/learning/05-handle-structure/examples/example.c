#include <stdio.h>
#include "../src/handle.h"
#include "../04-loop-structure/src/event_loop.h"

/*
 * Example: Using handle structure.
 * 
 * This demonstrates how handles work as the base structure
 * for all handle types in the event loop.
 */

static void close_callback(struct handle* handle) {
  printf("Handle closed: %s\n", handle_type_name(handle_get_type(handle)));
}

int main(void) {
  struct event_loop loop;
  struct handle idle_handle;
  struct handle timer_handle;
  struct handle io_handle;
  
  /* Initialize event loop */
  event_loop_init(&loop);
  
  /* Initialize different handle types */
  handle_init(&idle_handle, &loop, HANDLE_TYPE_IDLE);
  handle_init(&timer_handle, &loop, HANDLE_TYPE_TIMER);
  handle_init(&io_handle, &loop, HANDLE_TYPE_IO);
  
  /* Set user data */
  int idle_data = 1;
  int timer_data = 2;
  int io_data = 3;
  
  handle_set_data(&idle_handle, &idle_data);
  handle_set_data(&timer_handle, &timer_data);
  handle_set_data(&io_handle, &io_data);
  
  /* Activate handles */
  handle_set_active(&idle_handle);
  handle_set_active(&timer_handle);
  
  printf("Handle Information:\n");
  printf("  Idle handle: type=%s, active=%d, data=%d\n",
         handle_type_name(handle_get_type(&idle_handle)),
         handle_is_active(&idle_handle),
         *(int*)handle_get_data(&idle_handle));
  
  printf("  Timer handle: type=%s, active=%d, data=%d\n",
         handle_type_name(handle_get_type(&timer_handle)),
         handle_is_active(&timer_handle),
         *(int*)handle_get_data(&timer_handle));
  
  printf("  IO handle: type=%s, active=%d, data=%d\n",
         handle_type_name(handle_get_type(&io_handle)),
         handle_is_active(&io_handle),
         *(int*)handle_get_data(&io_handle));
  
  /* Start closing a handle */
  handle_start_closing(&idle_handle, close_callback);
  printf("\nClosing idle handle...\n");
  printf("  Is closing: %d\n", handle_is_closing(&idle_handle));
  
  /* Cleanup */
  event_loop_free(&loop);
  
  return 0;
}

