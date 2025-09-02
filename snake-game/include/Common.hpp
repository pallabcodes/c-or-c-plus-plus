#pragma once

#include <vector>
#include <utility>
#include <chrono>
#include <memory>

namespace SnakeGame {

// Game constants
constexpr int DEFAULT_BOARD_WIDTH = 20;
constexpr int DEFAULT_BOARD_HEIGHT = 20;
constexpr int MIN_BOARD_SIZE = 10;
constexpr int MAX_BOARD_SIZE = 50;
constexpr int DEFAULT_FPS = 30;
constexpr int MIN_FPS = 15;
constexpr int MAX_FPS = 60;

// Scoring constants
constexpr int POINTS_PER_FOOD = 10;
constexpr int CHECKPOINT_INTERVAL = 100; // points
constexpr int MAX_CHECKPOINTS = 5;

// Game states
enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    GAME_OVER
};

// Direction enum
enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// Position type (x, y coordinates)
using Position = std::pair<int, int>;

// Snake body type
using SnakeBody = std::vector<Position>;

// Game configuration
struct GameConfig {
    int boardWidth = DEFAULT_BOARD_WIDTH;
    int boardHeight = DEFAULT_BOARD_HEIGHT;
    int fps = DEFAULT_FPS;
    bool enableCheckpoints = true;
    bool enableHighScore = true;
};

// Checkpoint data structure
struct CheckpointData {
    SnakeBody snakeBody;
    Position foodPosition;
    int score;
    int highScore;
    GameState gameState;
    std::chrono::system_clock::time_point timestamp;
};

// Forward declarations
class Game;
class Snake;
class Food;
class Board;
class ScoreManager;
class CheckpointManager;

} // namespace SnakeGame
