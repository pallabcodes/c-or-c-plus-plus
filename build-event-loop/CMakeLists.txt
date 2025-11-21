cmake_minimum_required(VERSION 3.15)
project(build_event_loop C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Get the parent directory (workspace root)
get_filename_component(WORKSPACE_ROOT "${CMAKE_SOURCE_DIR}/.." ABSOLUTE)

# Node.js/libuv include paths
set(NODE_ROOT "${WORKSPACE_ROOT}/node")
set(LIBUV_SRC_DIR "${NODE_ROOT}/deps/uv/src")
set(LIBUV_INCLUDE_DIR "${NODE_ROOT}/deps/uv/include")

# Make libuv headers available globally
include_directories(
  ${LIBUV_INCLUDE_DIR}
  ${LIBUV_SRC_DIR}
  ${LIBUV_SRC_DIR}/unix
)

# Export these paths for subdirectories
set(LIBUV_SRC_DIR ${LIBUV_SRC_DIR} CACHE PATH "libuv source directory")
set(LIBUV_INCLUDE_DIR ${LIBUV_INCLUDE_DIR} CACHE PATH "libuv include directory")

add_subdirectory(minimals/epoll_basics)
add_subdirectory(minimals/kqueue_basics)
add_subdirectory(minimals/timer_min_heap)
add_subdirectory(minimals/timer_wheel)
add_subdirectory(minimals/spsc_queue)
add_subdirectory(minimals/reactor_single_core)
add_subdirectory(minimals/edge_level_poller)
add_subdirectory(minimals/backpressure_watermarks)
add_subdirectory(minimals/writev_scatter_gather)
add_subdirectory(minimals/reuseport_accept_shard)
add_subdirectory(minimals/fairness_scheduler)
add_subdirectory(minimals/slow_consumer_policy)
