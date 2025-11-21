cmake_minimum_required(VERSION 3.10)
project(min_heap)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Source files
set(HEAP_SOURCES
  src/heap.h
  src/heap.c
)

# Test executable
add_executable(test_heap tests/test_heap.c src/heap.c)
target_include_directories(test_heap PRIVATE ${CMAKE_SOURCE_DIR})

# Example executable
add_executable(example examples/example.c src/heap.c)
target_include_directories(example PRIVATE ${CMAKE_SOURCE_DIR})

# Enable testing
enable_testing()
add_test(NAME HeapTests COMMAND test_heap)

