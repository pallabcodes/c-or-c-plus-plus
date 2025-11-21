#include "handle.h"
#include <string.h>

void handle_init(struct handle* handle,
                 struct event_loop* loop,
                 handle_type_t type) {
  if (handle == NULL || loop == NULL) {
    return;
  }
  
  memset(handle, 0, sizeof(struct handle));
  
  handle->loop = loop;
  handle->type = type;
  handle->data = NULL;
  handle->close_cb = NULL;
  handle->flags = 0;
  
  /* Initialize queue node */
  queue_init(&handle->handle_queue);
  
  /* Initialize union */
  handle->u.fd = -1;
}

int handle_is_active(const struct handle* handle) {
  if (handle == NULL) {
    return 0;
  }
  return (handle->flags & HANDLE_FLAG_ACTIVE) != 0;
}

int handle_is_closing(const struct handle* handle) {
  if (handle == NULL) {
    return 0;
  }
  return (handle->flags & HANDLE_FLAG_CLOSING) != 0;
}

int handle_is_closed(const struct handle* handle) {
  if (handle == NULL) {
    return 0;
  }
  return (handle->flags & HANDLE_FLAG_CLOSED) != 0;
}

void handle_set_active(struct handle* handle) {
  if (handle == NULL) {
    return;
  }
  handle->flags |= HANDLE_FLAG_ACTIVE;
}

void handle_set_inactive(struct handle* handle) {
  if (handle == NULL) {
    return;
  }
  handle->flags &= ~HANDLE_FLAG_ACTIVE;
}

void handle_start_closing(struct handle* handle, handle_close_cb close_cb) {
  if (handle == NULL) {
    return;
  }
  
  handle->flags |= HANDLE_FLAG_CLOSING;
  handle->close_cb = close_cb;
}

handle_type_t handle_get_type(const struct handle* handle) {
  if (handle == NULL) {
    return HANDLE_TYPE_UNKNOWN;
  }
  return handle->type;
}

void* handle_get_data(const struct handle* handle) {
  if (handle == NULL) {
    return NULL;
  }
  return handle->data;
}

void handle_set_data(struct handle* handle, void* data) {
  if (handle == NULL) {
    return;
  }
  handle->data = data;
}

struct event_loop* handle_get_loop(const struct handle* handle) {
  if (handle == NULL) {
    return NULL;
  }
  return handle->loop;
}

const char* handle_type_name(handle_type_t type) {
  switch (type) {
    case HANDLE_TYPE_UNKNOWN:
      return "UNKNOWN";
    case HANDLE_TYPE_IDLE:
      return "IDLE";
    case HANDLE_TYPE_PREPARE:
      return "PREPARE";
    case HANDLE_TYPE_CHECK:
      return "CHECK";
    case HANDLE_TYPE_TIMER:
      return "TIMER";
    case HANDLE_TYPE_IO:
      return "IO";
    default:
      return "INVALID";
  }
}

