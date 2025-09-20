cmake_minimum_required(VERSION 3.16)
project(SnakeGame VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -O2")
endif()

# Find SDL2
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_TTF_INCLUDE_DIRS})

# Source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Snake.cpp
    src/Food.cpp
    src/Board.cpp
    src/ScoreManager.cpp
    src/CheckpointManager.cpp
    src/Renderer.cpp
)

# Header files
set(HEADERS
    include/Game.hpp
    include/Snake.hpp
    include/Food.hpp
    include/Board.hpp
    include/ScoreManager.hpp
    include/CheckpointManager.hpp
    include/Common.hpp
    include/Renderer.hpp
)

# Create executable
add_executable(SnakeGame ${SOURCES} ${HEADERS})

# Link SDL2 libraries
target_link_libraries(SnakeGame ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})
target_compile_options(SnakeGame PRIVATE ${SDL2_CFLAGS_OTHER})

# Set properties
set_target_properties(SnakeGame PROPERTIES
    OUTPUT_NAME "snake"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Print configuration info
message(STATUS "Snake Game Configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Output Directory: ${CMAKE_BINARY_DIR}/bin")
message(STATUS "  SDL2 Version: ${SDL2_VERSION}")
message(STATUS "  SDL2_TTF Version: ${SDL2_TTF_VERSION}")
