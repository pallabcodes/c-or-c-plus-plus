cmake_minimum_required(VERSION 3.20)
project(NetworkingLibrary VERSION 1.0.0 LANGUAGES CXX)

# Set C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific optimizations
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -mtune=native")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -fsanitize=address -fsanitize=undefined")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fsanitize=address -fsanitize=undefined")
endif()

# Warning flags for production-quality code
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-unused-variable")

# Options
option(ENABLE_BENCHMARKS "Enable performance benchmarks" OFF)
option(ENABLE_TLS "Enable TLS/SSL support" ON)
option(ENABLE_COMPRESSION "Enable compression support" ON)

# Find required packages
find_package(Threads REQUIRED)

# OpenSSL for TLS support
if(ENABLE_TLS)
    find_package(OpenSSL REQUIRED)
    add_definitions(-DENABLE_TLS=1)
endif()

# Google Test for unit testing
find_package(GTest)
if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9fd7e7b3673be2a1dc0d050.zip
    )
    FetchContent_MakeAvailable(googletest)
endif()

# Google Benchmark for performance testing
if(ENABLE_BENCHMARKS)
    find_package(benchmark)
    if(NOT benchmark_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            googlebenchmark
            URL https://github.com/google/benchmark/archive/v1.8.3.zip
        )
        FetchContent_MakeAvailable(googlebenchmark)
    endif()
endif()

# Include directories
include_directories(include)

# Create main networking library
file(GLOB_RECURSE NETWORKING_SOURCES 
    "src/network/*.cpp"
    "src/http/*.cpp" 
    "src/websocket/*.cpp"
    "src/utils/*.cpp"
)

add_library(networking STATIC ${NETWORKING_SOURCES})
target_include_directories(networking PUBLIC include)
target_link_libraries(networking 
    PUBLIC Threads::Threads
    PRIVATE $<$<BOOL:${ENABLE_TLS}>:OpenSSL::SSL OpenSSL::Crypto>
)

# Examples (if they exist)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/production_server.cpp")
    add_executable(production_server examples/production_server.cpp)
    target_link_libraries(production_server networking)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/real_time_chat.cpp")
    add_executable(real_time_chat examples/real_time_chat.cpp)
    target_link_libraries(real_time_chat networking)
endif()

# HTTP Demo Examples
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/http_server_demo.cpp")
    add_executable(http_server_demo examples/http_server_demo.cpp)
    target_link_libraries(http_server_demo networking)
    message(STATUS "✅ HTTP Server Demo will be built")
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/http_client_test.cpp")
    add_executable(http_client_test examples/http_client_test.cpp)
    target_link_libraries(http_client_test networking)
    message(STATUS "✅ HTTP Client Test will be built")
endif()

# Tests (if they exist)
if(GTest_FOUND OR TARGET gtest)
    enable_testing()
    
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_networking.cpp")
        add_executable(test_networking tests/test_networking.cpp)
        target_link_libraries(test_networking 
            networking 
            $<IF:$<TARGET_EXISTS:gtest>,gtest gtest_main,GTest::gtest GTest::gtest_main>
        )
        
        add_test(NAME networking_tests COMMAND test_networking)
    endif()
endif()

# Benchmarks
if(ENABLE_BENCHMARKS AND (benchmark_FOUND OR TARGET benchmark::benchmark))
    add_executable(http_benchmark benchmarks/http_benchmark.cpp)
    target_link_libraries(http_benchmark 
        networking 
        $<IF:$<TARGET_EXISTS:benchmark::benchmark>,benchmark::benchmark,benchmark>
    )
    
    add_executable(websocket_benchmark benchmarks/websocket_benchmark.cpp)
    target_link_libraries(websocket_benchmark 
        networking 
        $<IF:$<TARGET_EXISTS:benchmark::benchmark>,benchmark::benchmark,benchmark>
    )
endif()

# Installation
install(TARGETS networking 
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ 
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# Documentation with Doxygen (optional)
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs)
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_EXTRACT_PRIVATE YES)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_MAN NO)
    
    doxygen_add_docs(docs
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/docs
        COMMENT "Generating API documentation with Doxygen"
    )
endif()

# Print configuration summary
message(STATUS "=== Configuration Summary ===")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "TLS Support: ${ENABLE_TLS}")
message(STATUS "Benchmarks: ${ENABLE_BENCHMARKS}")
message(STATUS "Compression: ${ENABLE_COMPRESSION}")
message(STATUS "=============================")
