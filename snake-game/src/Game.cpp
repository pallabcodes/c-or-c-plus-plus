#include "Game.hpp"
#include "Snake.hpp"
#include "Food.hpp"
#include "Board.hpp"
#include "ScoreManager.hpp"
#include "CheckpointManager.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <limits>



namespace SnakeGame {

Game::Game(const GameConfig& config)
    : currentState_(GameState::MENU)
    , config_(config)
    , actualFPS_(config.fps) {
    
    // Initialize game components
    snake_ = std::make_unique<Snake>(Position{config.boardWidth / 2, config.boardHeight / 2});
    food_ = std::make_unique<Food>();
    board_ = std::make_unique<Board>(config.boardWidth, config.boardHeight);
    scoreManager_ = std::make_unique<ScoreManager>();
    checkpointManager_ = std::make_unique<CheckpointManager>();
    
    // Calculate window size based on board size and cell size
    int cellSize = 20; // 20 pixels per cell
    int windowWidth = config.boardWidth * cellSize;
    int windowHeight = config.boardHeight * cellSize + 100; // Extra space for score display
    renderer_ = std::make_unique<Renderer>(windowWidth, windowHeight);
}

void Game::initialize() {
    // Initialize renderer first
    if (!renderer_->initialize()) {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return;
    }
    
    board_->initialize();
    
    // Place initial food
    food_->generateNewPosition(*board_, snake_->getBody());
    
    // Initialize timing
    lastFrameTime_ = std::chrono::steady_clock::now();
    lastUpdateTime_ = lastFrameTime_;
}

void Game::run() {
    initialize();
    
    while (currentState_ != GameState::MENU) {
        auto currentTime = std::chrono::steady_clock::now();
        
        // Process input
        processInput();
        
        // Update game logic
        if (currentState_ == GameState::PLAYING) {
            auto timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(
                currentTime - lastUpdateTime_).count();
            
            if (timeSinceLastUpdate >= (1000 / config_.fps)) {
                update();
                lastUpdateTime_ = currentTime;
            }
        }
        
        // Render
        render();
        
        // Calculate FPS
        calculateFPS();
        
        // Wait for next frame
        waitForFrame();
        
        lastFrameTime_ = currentTime;
    }
}

void Game::pause() {
    if (currentState_ == GameState::PLAYING) {
        setState(GameState::PAUSED);
    }
}

void Game::resume() {
    if (currentState_ == GameState::PAUSED) {
        setState(GameState::PLAYING);
    }
}

void Game::gameOver() {
    setState(GameState::GAME_OVER);
}

void Game::restart() {
    // Reset game components
    snake_->reset(Position{config_.boardWidth / 2, config_.boardHeight / 2});
    food_->generateNewPosition(*board_, snake_->getBody());
    scoreManager_->resetScore();
    
    setState(GameState::PLAYING);
}

void Game::update() {
    // Move snake
    snake_->move();
    
    // Check collisions
    checkCollisions();
    
    // Update game logic
    updateGameLogic();
    
    // Check if we should create a checkpoint
    if (checkpointManager_->shouldCreateCheckpoint(scoreManager_->getCurrentScore())) {
        createCheckpoint();
    }
}

void Game::render() {
    if (!renderer_->isInitialized()) return;
    
    // Clear the screen
    renderer_->clear();
    
    // Calculate cell size
    int cellSize = 20;
    
    // Render the game board
    renderer_->renderBoard(board_->getWidth(), board_->getHeight(), cellSize);
    
    // Render the snake
    renderer_->renderSnake(snake_->getBody(), cellSize);
    
    // Render the food
    renderer_->renderFood(food_->getPosition(), cellSize);
    
    // Render score
    renderer_->renderScore(scoreManager_->getCurrentScore(), scoreManager_->getHighScore());
    
    // Render game state text
    switch (currentState_) {
        case GameState::MENU:
            renderer_->drawText("Press SPACE to start", renderer_->getWidth() / 2 - 100, renderer_->getHeight() - 50);
            break;
        case GameState::PAUSED:
            renderer_->drawText("PAUSED - Press SPACE to resume", renderer_->getWidth() / 2 - 150, renderer_->getHeight() - 50);
            break;
        case GameState::GAME_OVER:
            renderer_->drawText("GAME OVER - Press SPACE to restart", renderer_->getWidth() / 2 - 150, renderer_->getHeight() - 50);
            break;
        default:
            break;
    }
    
    // Present the rendered frame
    renderer_->present();
}

void Game::processInput() {
    // Input is now handled in the main loop with SDL2 events
    // This method is kept for compatibility but no longer used
}

void Game::handleKeyPress(int key) {
    // This method is deprecated - use handleInput(SDL_Keycode) instead
    (void)key; // Suppress unused parameter warning
}

void Game::setState(GameState newState) {
    currentState_ = newState;
}

GameState Game::getState() const {
    return currentState_;
}

const GameConfig& Game::getConfig() const {
    return config_;
}

void Game::setConfig(const GameConfig& config) {
    config_ = config;
}

bool Game::isRunning() const {
    return currentState_ == GameState::PLAYING;
}

bool Game::isPaused() const {
    return currentState_ == GameState::PAUSED;
}

int Game::getFPS() const {
    return actualFPS_;
}

void Game::createCheckpoint() {
    checkpointManager_->createCheckpoint(*snake_, *food_, *scoreManager_, currentState_);
}

bool Game::loadFromCheckpoint(size_t index) {
    return checkpointManager_->restoreFromCheckpoint(index, *snake_, *food_, *scoreManager_, currentState_);
}

bool Game::hasCheckpoints() const {
    return checkpointManager_->hasCheckpoints();
}

void Game::handleFoodCollision() {
    // Add points
    scoreManager_->addPoints(POINTS_PER_FOOD);
    
    // Grow snake
    snake_->grow();
    
    // Generate new food
    food_->generateNewPosition(*board_, snake_->getBody());
}

void Game::checkCollisions() {
    Position head = snake_->getHead();
    
    // Check wall collision
    if (board_->isWallCollision(head)) {
        gameOver();
        return;
    }
    
    // Check self collision
    if (snake_->checkSelfCollision()) {
        gameOver();
        return;
    }
    
    // Check food collision
    if (food_->isEaten(head)) {
        handleFoodCollision();
    }
}

void Game::updateGameLogic() {
    // Additional game logic can be added here
    // For now, just basic movement and collision detection
}

void Game::renderGameInfo() {
    // Display score and high score
    std::cout << "\n" << scoreManager_->getScoreText() << " | " 
              << scoreManager_->getHighScoreText() << std::endl;
    
    // Display FPS
    std::cout << "FPS: " << actualFPS_ << std::endl;
    
    // Display controls
    std::cout << "Controls: Arrow Keys to move, P to pause, ESC for menu" << std::endl;
}



void Game::handleMenuInput(int key) {
    switch (key) {
        case '1':
        case 's':
        case 'S':
            if (currentState_ == GameState::MENU) {
                setState(GameState::PLAYING);
            }
            break;
        case '2':
        case 'c':
        case 'C':
            if (currentState_ == GameState::MENU && hasCheckpoints()) {
                loadFromCheckpoint();
                setState(GameState::PLAYING);
            }
            break;
        case '3':
        case 'q':
        case 'Q':
        case 27: // ESC
            if (currentState_ == GameState::GAME_OVER) {
                setState(GameState::MENU);
            }
            break;
    }
}

void Game::handleInput(SDL_Keycode key) {
    switch (key) {
        case SDLK_w:
        case SDLK_UP:
            if (currentState_ == GameState::PLAYING) {
                snake_->changeDirection(Direction::UP);
            }
            break;
        case SDLK_s:
        case SDLK_DOWN:
            if (currentState_ == GameState::PLAYING) {
                snake_->changeDirection(Direction::DOWN);
            }
            break;
        case SDLK_a:
        case SDLK_LEFT:
            if (currentState_ == GameState::PLAYING) {
                snake_->changeDirection(Direction::LEFT);
            }
            break;
        case SDLK_d:
        case SDLK_RIGHT:
            if (currentState_ == GameState::PLAYING) {
                snake_->changeDirection(Direction::RIGHT);
            }
            break;
        case SDLK_SPACE:
            if (currentState_ == GameState::MENU) {
                setState(GameState::PLAYING);
            } else if (currentState_ == GameState::PLAYING) {
                pause();
            } else if (currentState_ == GameState::PAUSED) {
                resume();
            } else if (currentState_ == GameState::GAME_OVER) {
                restart();
            }
            break;
        case SDLK_ESCAPE:
            if (currentState_ == GameState::PLAYING) {
                pause();
            } else if (currentState_ == GameState::PAUSED) {
                setState(GameState::MENU);
            }
            break;
    }
}

void Game::showMainMenu() {
    std::cout << "\n=== SNAKE GAME ===" << std::endl;
    std::cout << "1. Start New Game" << std::endl;
    if (hasCheckpoints()) {
        std::cout << "2. Continue from Checkpoint" << std::endl;
    }
    std::cout << "3. Exit" << std::endl;
    std::cout << "Select option: ";
}

void Game::showGameOverMenu() {
    std::cout << "\n=== GAME OVER ===" << std::endl;
    std::cout << "Final Score: " << scoreManager_->getCurrentScore() << std::endl;
    if (scoreManager_->isNewHighScore()) {
        std::cout << "NEW HIGH SCORE!" << std::endl;
    }
    std::cout << "1. Play Again" << std::endl;
    std::cout << "2. Return to Menu" << std::endl;
    std::cout << "Select option: ";
}

void Game::showPauseMenu() {
    std::cout << "\n=== PAUSED ===" << std::endl;
    std::cout << "Press P to resume or ESC for menu" << std::endl;
}

void Game::calculateFPS() {
    auto currentTime = std::chrono::steady_clock::now();
    auto frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - lastFrameTime_).count();
    
    if (frameTime > 0) {
        actualFPS_ = 1000 / frameTime;
    }
}

void Game::clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void Game::waitForFrame() {
    auto currentTime = std::chrono::steady_clock::now();
    auto frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - lastFrameTime_).count();
    
    int targetFrameTime = 1000 / config_.fps;
    if (frameTime < targetFrameTime) {
        std::this_thread::sleep_for(std::chrono::milliseconds(targetFrameTime - frameTime));
    }
}



} // namespace SnakeGame
