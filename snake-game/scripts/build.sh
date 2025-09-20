#!/bin/bash

# Snake Game Build Script
# This script builds the Snake game with proper dependencies

set -e  # Exit on any error

echo "🐍 Building Snake Game..."

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "❌ Error: CMakeLists.txt not found. Please run this script from the snake-game directory."
    exit 1
fi

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Check if SDL2 is available
if ! pkg-config --exists sdl2; then
    echo "❌ Error: SDL2 not found. Please install SDL2 development libraries."
    echo "   Ubuntu/Debian: sudo apt-get install libsdl2-dev libsdl2-ttf-dev"
    echo "   macOS: brew install sdl2 sdl2_ttf"
    exit 1
fi

if ! pkg-config --exists SDL2_ttf; then
    echo "❌ Error: SDL2_ttf not found. Please install SDL2_ttf development libraries."
    echo "   Ubuntu/Debian: sudo apt-get install libsdl2-ttf-dev"
    echo "   macOS: brew install sdl2_ttf"
    exit 1
fi

echo "✅ SDL2 dependencies found"

# Configure with CMake
echo "🔧 Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "🏗️  Building project..."
make -j$(nproc)

echo "✅ Build completed successfully!"
echo "🎮 Run the game with: ./build/bin/snake"
