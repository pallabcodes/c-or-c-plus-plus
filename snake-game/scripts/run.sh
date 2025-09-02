#!/bin/bash

# Snake Game Run Script
# This script runs the Snake game

echo "🐍 Launching Snake Game..."

# Check if the game executable exists
if [ ! -f "build/bin/snake" ]; then
    echo "❌ Error: Game executable not found. Please build the game first:"
    echo "   ./scripts/build.sh"
    exit 1
fi

# Check if we have a display
if [ -z "$DISPLAY" ]; then
    echo "❌ Error: No display available. Make sure you're running in a graphical environment."
    exit 1
fi

echo "✅ Starting Snake Game..."
echo "🎮 Controls:"
echo "   - Movement: Arrow keys or WASD"
echo "   - Pause: Spacebar or ESC"
echo "   - Start: Spacebar from menu"
echo "   - Exit: ESC key"

# Run the game
cd build/bin
./snake

echo "👋 Thanks for playing Snake Game!"
