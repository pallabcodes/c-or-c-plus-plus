cmake_minimum_required(VERSION 3.10)
project(time_management)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Source files
set(TIME_SOURCES
  src/time.h
  src/time.c
)

# Test executable
add_executable(test_time tests/test_time.c src/time.c)
target_include_directories(test_time PRIVATE ${CMAKE_SOURCE_DIR})

# Example executable
add_executable(example examples/example.c src/time.c)
target_include_directories(example PRIVATE ${CMAKE_SOURCE_DIR})

# Enable testing
enable_testing()
add_test(NAME TimeTests COMMAND test_time)

