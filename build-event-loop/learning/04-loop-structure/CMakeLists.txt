cmake_minimum_required(VERSION 3.10)
project(event_loop_structure)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(
  ../01-intrusive-queue/src
  ../02-min-heap/src
  ../03-time-management/src
)

# Source files
set(LOOP_SOURCES
  src/event_loop.h
  src/event_loop.c
)

# Dependencies
set(HEAP_SOURCES ../02-min-heap/src/heap.c)
set(TIME_SOURCES ../03-time-management/src/time.c)

# Test executable
add_executable(test_event_loop 
  tests/test_event_loop.c 
  src/event_loop.c 
  ${HEAP_SOURCES} 
  ${TIME_SOURCES})
target_include_directories(test_event_loop PRIVATE ${CMAKE_SOURCE_DIR})

# Enable testing
enable_testing()
add_test(NAME EventLoopTests COMMAND test_event_loop)

