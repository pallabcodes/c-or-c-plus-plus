#include "CheckpointManager.hpp"
#include "Snake.hpp"
#include "Food.hpp"
#include "ScoreManager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace SnakeGame {

CheckpointManager::CheckpointManager() {
    loadCheckpoints();
}

bool CheckpointManager::hasCheckpoints() const {
    return !checkpoints_.empty();
}

int CheckpointManager::getCheckpointCount() const {
    return checkpoints_.size();
}

const std::vector<CheckpointData>& CheckpointManager::getCheckpoints() const {
    return checkpoints_;
}

bool CheckpointManager::shouldCreateCheckpoint(int currentScore) const {
    return currentScore > 0 && (currentScore % CHECKPOINT_INTERVAL == 0);
}

void CheckpointManager::createCheckpoint(const Snake& snake, const Food& food, 
                                       const ScoreManager& scoreManager, GameState gameState) {
    CheckpointData checkpoint;
    checkpoint.snakeBody = snake.getBody();
    checkpoint.foodPosition = food.getPosition();
    checkpoint.score = scoreManager.getCurrentScore();
    checkpoint.highScore = scoreManager.getHighScore();
    checkpoint.gameState = gameState;
    checkpoint.timestamp = std::chrono::system_clock::now();
    
    checkpoints_.push_back(checkpoint);
    
    // Keep only the most recent checkpoints
    if (checkpoints_.size() > MAX_CHECKPOINTS) {
        removeOldCheckpoints();
    }
    
    // Save checkpoints to file
    saveCheckpoints();
}

bool CheckpointManager::restoreFromCheckpoint(size_t index, Snake& snake, Food& food, 
                                            ScoreManager& scoreManager, GameState& gameState) {
    if (index >= checkpoints_.size()) {
        return false;
    }
    
    const CheckpointData& checkpoint = checkpoints_[index];
    if (!isValidCheckpoint(checkpoint)) {
        return false;
    }
    
    // Restore snake
    snake.reset(checkpoint.snakeBody.front());
    // Note: This is a simplified restoration. In a full implementation,
    // we'd need to properly restore the snake's body and direction
    
    // Restore food
    food.setPosition(checkpoint.foodPosition);
    
    // Restore score
    scoreManager.setCheckpointScore(checkpoint.score);
    
    // Restore game state
    gameState = checkpoint.gameState;
    
    return true;
}

bool CheckpointManager::restoreFromLastCheckpoint(Snake& snake, Food& food, 
                                                ScoreManager& scoreManager, GameState& gameState) {
    if (checkpoints_.empty()) {
        return false;
    }
    
    return restoreFromCheckpoint(checkpoints_.size() - 1, snake, food, scoreManager, gameState);
}

void CheckpointManager::saveCheckpoints() {
    std::ofstream file(CHECKPOINT_FILENAME, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening checkpoint file for writing" << std::endl;
        return;
    }
    
    // Write number of checkpoints
    size_t count = checkpoints_.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
    
    // Write each checkpoint
    for (const auto& checkpoint : checkpoints_) {
        serializeCheckpoint(checkpoint, file);
    }
    
    file.close();
}

void CheckpointManager::loadCheckpoints() {
    std::ifstream file(CHECKPOINT_FILENAME, std::ios::binary);
    if (!file.is_open()) {
        // File doesn't exist, start with empty checkpoints
        return;
    }
    
    checkpoints_.clear();
    
    // Read number of checkpoints
    size_t count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));
    
    // Read each checkpoint
    for (size_t i = 0; i < count; ++i) {
        CheckpointData checkpoint = deserializeCheckpoint(file);
        if (isValidCheckpoint(checkpoint)) {
            checkpoints_.push_back(checkpoint);
        }
    }
    
    file.close();
}

void CheckpointManager::clearCheckpoints() {
    checkpoints_.clear();
    saveCheckpoints();
}

std::string CheckpointManager::getCheckpointInfo(size_t index) const {
    if (index >= checkpoints_.size()) {
        return "Invalid checkpoint index";
    }
    
    const CheckpointData& checkpoint = checkpoints_[index];
    std::ostringstream oss;
    
    oss << "Checkpoint " << (index + 1) << ": ";
    oss << "Score: " << checkpoint.score << ", ";
    oss << "Snake Length: " << checkpoint.snakeBody.size();
    
    return oss.str();
}

void CheckpointManager::removeOldCheckpoints() {
    if (checkpoints_.size() <= MAX_CHECKPOINTS) {
        return;
    }
    
    // Remove oldest checkpoints
    size_t toRemove = checkpoints_.size() - MAX_CHECKPOINTS;
    checkpoints_.erase(checkpoints_.begin(), checkpoints_.begin() + toRemove);
}

void CheckpointManager::serializeCheckpoint(const CheckpointData& data, std::ofstream& file) {
    // Serialize snake body
    size_t bodySize = data.snakeBody.size();
    file.write(reinterpret_cast<const char*>(&bodySize), sizeof(bodySize));
    
    for (const auto& pos : data.snakeBody) {
        file.write(reinterpret_cast<const char*>(&pos.first), sizeof(pos.first));
        file.write(reinterpret_cast<const char*>(&pos.second), sizeof(pos.second));
    }
    
    // Serialize food position
    file.write(reinterpret_cast<const char*>(&data.foodPosition.first), sizeof(data.foodPosition.first));
    file.write(reinterpret_cast<const char*>(&data.foodPosition.second), sizeof(data.foodPosition.second));
    
    // Serialize score and high score
    file.write(reinterpret_cast<const char*>(&data.score), sizeof(data.score));
    file.write(reinterpret_cast<const char*>(&data.highScore), sizeof(data.highScore));
    
    // Serialize game state
    int gameStateInt = static_cast<int>(data.gameState);
    file.write(reinterpret_cast<const char*>(&gameStateInt), sizeof(gameStateInt));
    
    // Serialize timestamp
    auto timePoint = data.timestamp.time_since_epoch().count();
    file.write(reinterpret_cast<const char*>(&timePoint), sizeof(timePoint));
}

CheckpointData CheckpointManager::deserializeCheckpoint(std::ifstream& file) {
    CheckpointData data;
    
    // Deserialize snake body
    size_t bodySize;
    file.read(reinterpret_cast<char*>(&bodySize), sizeof(bodySize));
    
    data.snakeBody.resize(bodySize);
    for (size_t i = 0; i < bodySize; ++i) {
        file.read(reinterpret_cast<char*>(&data.snakeBody[i].first), sizeof(data.snakeBody[i].first));
        file.read(reinterpret_cast<char*>(&data.snakeBody[i].second), sizeof(data.snakeBody[i].second));
    }
    
    // Deserialize food position
    file.read(reinterpret_cast<char*>(&data.foodPosition.first), sizeof(data.foodPosition.first));
    file.read(reinterpret_cast<char*>(&data.foodPosition.second), sizeof(data.foodPosition.second));
    
    // Deserialize score and high score
    file.read(reinterpret_cast<char*>(&data.score), sizeof(data.score));
    file.read(reinterpret_cast<char*>(&data.highScore), sizeof(data.highScore));
    
    // Deserialize game state
    int gameStateInt;
    file.read(reinterpret_cast<char*>(&gameStateInt), sizeof(gameStateInt));
    data.gameState = static_cast<GameState>(gameStateInt);
    
    // Deserialize timestamp
    std::chrono::system_clock::rep timePoint;
    file.read(reinterpret_cast<char*>(&timePoint), sizeof(timePoint));
    data.timestamp = std::chrono::system_clock::from_time_t(timePoint);
    
    return data;
}

bool CheckpointManager::isValidCheckpoint(const CheckpointData& data) const {
    // Basic validation
    if (data.snakeBody.empty()) {
        return false;
    }
    
    if (data.score < 0) {
        return false;
    }
    
    if (data.highScore < 0) {
        return false;
    }
    
    return true;
}

} // namespace SnakeGame
