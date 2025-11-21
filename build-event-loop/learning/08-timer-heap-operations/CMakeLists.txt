cmake_minimum_required(VERSION 3.10)
project(timer_heap_operations)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Source files
set(TIMER_HEAP_SOURCES
  src/timer_heap.h
  src/timer_heap.c
)

# Test executable
add_executable(test_timer_heap tests/test_timer_heap.c)
target_sources(test_timer_heap PRIVATE ${TIMER_HEAP_SOURCES})
target_include_directories(test_timer_heap PRIVATE 
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/../01-intrusive-queue/src
  ${CMAKE_SOURCE_DIR}/../02-min-heap/src
  ${CMAKE_SOURCE_DIR}/../03-time-management/src
  ${CMAKE_SOURCE_DIR}/../04-loop-structure/src
  ${CMAKE_SOURCE_DIR}/../05-handle-structure/src
  ${CMAKE_SOURCE_DIR}/../07-timer-structure/src
)

# Link dependencies
target_sources(test_timer_heap PRIVATE
  ../07-timer-structure/src/timer.c
  ../05-handle-structure/src/handle.c
  ../04-loop-structure/src/event_loop.c
  ../02-min-heap/src/heap.c
  ../03-time-management/src/time.c
)

# Example executable
add_executable(example examples/example.c)
target_sources(example PRIVATE ${TIMER_HEAP_SOURCES})
target_include_directories(example PRIVATE 
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/../01-intrusive-queue/src
  ${CMAKE_SOURCE_DIR}/../02-min-heap/src
  ${CMAKE_SOURCE_DIR}/../03-time-management/src
  ${CMAKE_SOURCE_DIR}/../04-loop-structure/src
  ${CMAKE_SOURCE_DIR}/../05-handle-structure/src
  ${CMAKE_SOURCE_DIR}/../07-timer-structure/src
)

target_sources(example PRIVATE
  ../07-timer-structure/src/timer.c
  ../05-handle-structure/src/handle.c
  ../04-loop-structure/src/event_loop.c
  ../02-min-heap/src/heap.c
  ../03-time-management/src/time.c
)

# Enable testing
enable_testing()
add_test(NAME TimerHeapTests COMMAND test_timer_heap)

