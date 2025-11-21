cmake_minimum_required(VERSION 3.10)
project(intrusive_queue)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Source files
set(QUEUE_SOURCES
  src/queue.h
)

# Test executable (uses local queue.h)
add_executable(test_queue tests/test_queue.c)
target_include_directories(test_queue PRIVATE ${CMAKE_SOURCE_DIR})

# Example executable (uses local queue.h)
add_executable(example examples/example.c)
target_include_directories(example PRIVATE ${CMAKE_SOURCE_DIR})

# Example using libuv's queue.h directly
add_executable(example_libuv examples/example_libuv.c)
target_include_directories(example_libuv PRIVATE 
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/../../../../node/deps/uv/src
  ${CMAKE_SOURCE_DIR}/../../../../node/deps/uv/include
)

# Enable testing
enable_testing()
add_test(NAME QueueTests COMMAND test_queue)

