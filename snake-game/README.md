# Snake Game

A classic Snake game implementation in C++ with SDL2 graphics, featuring a comprehensive point system and checkpoint functionality.

## Features

- **Classic Snake Gameplay**: Control a snake to eat food and grow longer
- **SDL2 Graphics**: Modern graphical interface with smooth rendering
- **Point System**: Score tracking with high score persistence
- **Checkpoint System**: Save and restore game progress
- **Cross-platform**: Works on Linux, Windows, and macOS
- **Modern C++**: Built with C++17 standards

## Game Controls

- **Movement**: Arrow keys or WASD
- **Pause**: Spacebar or ESC
- **Start Game**: Spacebar from menu
- **Exit**: ESC key

## Requirements

- C++17 compatible compiler
- SDL2 development libraries
- SDL2_ttf for text rendering
- CMake 3.16 or later

## Installation

### Ubuntu/Debian
```bash
sudo apt-get install libsdl2-dev libsdl2-ttf-dev cmake build-essential
```

### macOS
```bash
brew install sdl2 sdl2_ttf cmake
```

### Windows
- Install Visual Studio with C++ support
- Install SDL2 development libraries
- Install CMake

## Building

```bash
cd snake-game
mkdir build && cd build
cmake ..
make -j4
```

## Running

```bash
cd build/bin
./snake
```

## Project Structure

```
snake-game/
├── src/                    # Source files
│   ├── main.cpp           # Main entry point
│   ├── Game.cpp           # Game logic and state management
│   ├── Snake.cpp          # Snake entity implementation
│   ├── Food.cpp           # Food generation and management
│   ├── Board.cpp          # Game board and collision detection
│   ├── Renderer.cpp       # SDL2 rendering system
│   ├── ScoreManager.cpp   # Score tracking and persistence
│   └── CheckpointManager.cpp # Checkpoint system
├── include/                # Header files
│   ├── Game.hpp           # Game class interface
│   ├── Snake.hpp          # Snake class interface
│   ├── Food.hpp           # Food class interface
│   ├── Board.hpp          # Board class interface
│   ├── Renderer.hpp       # Renderer class interface
│   ├── ScoreManager.hpp   # Score manager interface
│   ├── CheckpointManager.hpp # Checkpoint manager interface
│   └── Common.hpp         # Common types and constants
├── docs/                   # Documentation
│   ├── prd.txt            # Product Requirements Document
│   └── tasks.txt          # Development tasks
├── build/                  # Build output directory
├── assets/                 # Game assets (future use)
├── scripts/                # Build and utility scripts
└── CMakeLists.txt         # CMake build configuration
```

## Game Architecture

The game follows a modular architecture with clear separation of concerns:

- **Game**: Main game loop and state management
- **Snake**: Snake entity with movement and growth logic
- **Food**: Food generation and positioning
- **Board**: Game board and collision detection
- **Renderer**: SDL2-based graphics rendering
- **ScoreManager**: Point calculation and tracking
- **CheckpointManager**: Checkpoint creation and restoration

## Development Status

✅ **Completed Features:**
- Core snake movement and growth mechanics
- Food generation and collision detection
- Wall and self-collision detection
- Basic point system with high score tracking
- Checkpoint system with save/load functionality
- SDL2-based graphical interface
- Cross-platform build system

🔄 **In Progress:**
- Game testing and bug fixes
- Performance optimization

📋 **Future Enhancements:**
- Advanced graphics and animations
- Sound effects and music
- Multiple difficulty levels
- Power-ups and special abilities
- Multiplayer support

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## License

This project is open source. Feel free to use, modify, and distribute according to your needs.

## Acknowledgments

- Built with SDL2 (Simple DirectMedia Layer)
- Inspired by the classic Snake game
- Developed as a learning project for modern C++ development
