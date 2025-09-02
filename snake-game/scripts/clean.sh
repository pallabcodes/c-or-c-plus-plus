#!/bin/bash

# Snake Game Clean Script
# This script cleans build artifacts

echo "🧹 Cleaning Snake Game build artifacts..."

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "❌ Error: CMakeLists.txt not found. Please run this script from the snake-game directory."
    exit 1
fi

# Remove build directory
if [ -d "build" ]; then
    echo "🗑️  Removing build directory..."
    rm -rf build
    echo "✅ Build directory removed"
else
    echo "ℹ️  No build directory found"
fi

# Remove any generated files
echo "🧹 Cleaning completed!"
echo "💡 To rebuild, run: ./scripts/build.sh"
