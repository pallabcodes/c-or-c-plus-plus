cmake_minimum_required(VERSION 3.10)
project(handle_structure)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Source files
set(HANDLE_SOURCES
  src/handle.h
  src/handle.c
)

# Test executable
add_executable(test_handle tests/test_handle.c)
target_sources(test_handle PRIVATE ${HANDLE_SOURCES})
target_include_directories(test_handle PRIVATE 
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/../01-intrusive-queue/src
  ${CMAKE_SOURCE_DIR}/../04-loop-structure/src
)

# Link dependencies
target_sources(test_handle PRIVATE
  ../04-loop-structure/src/event_loop.c
  ../02-min-heap/src/heap.c
  ../03-time-management/src/time.c
)

# Example executable
add_executable(example examples/example.c)
target_sources(example PRIVATE ${HANDLE_SOURCES})
target_include_directories(example PRIVATE 
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/../01-intrusive-queue/src
  ${CMAKE_SOURCE_DIR}/../04-loop-structure/src
)

target_sources(example PRIVATE
  ../04-loop-structure/src/event_loop.c
  ../02-min-heap/src/heap.c
  ../03-time-management/src/time.c
)

# Enable testing
enable_testing()
add_test(NAME HandleTests COMMAND test_handle)

