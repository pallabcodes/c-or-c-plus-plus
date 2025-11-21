#ifndef HANDLE_H_
#define HANDLE_H_

#include <stddef.h>
#include "../01-intrusive-queue/src/queue.h"
#include "../04-loop-structure/src/event_loop.h"

/*
 * Handle Structure - Base structure for all handle types.
 * 
 * This is the abstract base class for all handles in the event loop.
 * All handle types (idle, prepare, check, timer, I/O) inherit from this.
 * 
 * Based on libuv's uv_handle_t structure.
 * 
 * Source: node/deps/uv/include/uv.h (lines 465-483)
 */

/* Forward declaration */
struct event_loop;

/* Handle type enumeration */
typedef enum {
  HANDLE_TYPE_UNKNOWN = 0,
  HANDLE_TYPE_IDLE,
  HANDLE_TYPE_PREPARE,
  HANDLE_TYPE_CHECK,
  HANDLE_TYPE_TIMER,
  HANDLE_TYPE_IO,
  HANDLE_TYPE_MAX
} handle_type_t;

/* Close callback type */
typedef void (*handle_close_cb)(struct handle* handle);

/* Base handle structure */
struct handle {
  /* Public fields */
  void* data;                    /* User data pointer */
  
  /* Read-only fields */
  struct event_loop* loop;       /* Pointer to event loop */
  handle_type_t type;            /* Handle type */
  
  /* Private fields */
  handle_close_cb close_cb;      /* Close callback */
  struct queue handle_queue;     /* Queue node for linking handles */
  
  /* File descriptor or reserved space */
  union {
    int fd;                      /* File descriptor (for I/O handles) */
    void* reserved[4];           /* Reserved space (for non-I/O handles) */
  } u;
  
  /* Flags */
  unsigned int flags;            /* Internal flags (active, closing, etc.) */
};

/* Handle flags */
#define HANDLE_FLAG_ACTIVE    0x01  /* Handle is active */
#define HANDLE_FLAG_CLOSING   0x02  /* Handle is closing */
#define HANDLE_FLAG_CLOSED    0x04  /* Handle is closed */

/* Initialize a handle */
void handle_init(struct handle* handle, 
                 struct event_loop* loop,
                 handle_type_t type);

/* Check if handle is active */
int handle_is_active(const struct handle* handle);

/* Check if handle is closing */
int handle_is_closing(const struct handle* handle);

/* Check if handle is closed */
int handle_is_closed(const struct handle* handle);

/* Set handle as active */
void handle_set_active(struct handle* handle);

/* Set handle as inactive */
void handle_set_inactive(struct handle* handle);

/* Start closing a handle */
void handle_start_closing(struct handle* handle, handle_close_cb close_cb);

/* Get handle type */
handle_type_t handle_get_type(const struct handle* handle);

/* Get handle data */
void* handle_get_data(const struct handle* handle);

/* Set handle data */
void handle_set_data(struct handle* handle, void* data);

/* Get handle loop */
struct event_loop* handle_get_loop(const struct handle* handle);

/* Get handle type name (for debugging) */
const char* handle_type_name(handle_type_t type);

#endif /* HANDLE_H_ */

