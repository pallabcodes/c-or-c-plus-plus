cmake_minimum_required(VERSION 3.10)
project(io_watcher)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Source files
set(IO_WATCHER_SOURCES
  src/io_watcher.h
  src/io_watcher.c
)

# Test executable
add_executable(test_io_watcher tests/test_io_watcher.c)
target_sources(test_io_watcher PRIVATE ${IO_WATCHER_SOURCES})
target_include_directories(test_io_watcher PRIVATE 
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/../01-intrusive-queue/src
  ${CMAKE_SOURCE_DIR}/../04-loop-structure/src
)

# Link dependencies
target_sources(test_io_watcher PRIVATE
  ../04-loop-structure/src/event_loop.c
  ../02-min-heap/src/heap.c
  ../03-time-management/src/time.c
)

# Example executable
add_executable(example examples/example.c)
target_sources(example PRIVATE ${IO_WATCHER_SOURCES})
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
add_test(NAME IOWatcherTests COMMAND test_io_watcher)

