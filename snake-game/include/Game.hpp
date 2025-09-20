#pragma once

#include "Common.hpp"
#include "Snake.hpp"
#include "Food.hpp"
#include "Board.hpp"
#include "ScoreManager.hpp"
#include "CheckpointManager.hpp"
#include "Renderer.hpp"
#include <memory>
#include <chrono>

namespace SnakeGame {

class Game {
public:
    explicit Game(const GameConfig& config = GameConfig{});
    ~Game() = default;

    // Game lifecycle
    void initialize();
    void run();
    void pause();
    void resume();
    void gameOver();
    void restart();
    
    // Game loop
    void update();
    void render();
    void processInput();
    
    // State management
    GameState getState() const;
    void setState(GameState newState);
    
    // Configuration
    const GameConfig& getConfig() const;
    void setConfig(const GameConfig& config);
    
    // Checkpoint integration
    void createCheckpoint();
    bool loadFromCheckpoint(size_t index = 0);
    bool hasCheckpoints() const;
    
    // Input handling
    void handleInput(SDL_Keycode key);
    
    // Utility
    bool isRunning() const;
    bool isPaused() const;
    int getFPS() const;

private:
    // Game components
    std::unique_ptr<Snake> snake_;
    std::unique_ptr<Food> food_;
    std::unique_ptr<Board> board_;
    std::unique_ptr<ScoreManager> scoreManager_;
    std::unique_ptr<CheckpointManager> checkpointManager_;
    std::unique_ptr<Renderer> renderer_;
    
    // Game state
    GameState currentState_;
    GameConfig config_;
    
    // Timing
    std::chrono::steady_clock::time_point lastFrameTime_;
    std::chrono::steady_clock::time_point lastUpdateTime_;
    int actualFPS_;
    
    // Game logic
    void handleFoodCollision();
    void checkCollisions();
    void updateGameLogic();
    void renderGameInfo();
    
    // Input handling
    void handleKeyPress(int key);
    void handleMenuInput(int key);
    void handleGameplayInput(int key);
    
    // Menu system
    void showMainMenu();
    void showGameOverMenu();
    void showPauseMenu();
    
    // Utility
    void calculateFPS();
    void clearScreen();
    void waitForFrame();
};

} // namespace SnakeGame
