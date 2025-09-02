#include "ScoreManager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

namespace SnakeGame {

ScoreManager::ScoreManager() 
    : currentScore_(0)
    , highScore_(DEFAULT_HIGH_SCORE)
    , checkpointScore_(0) {
    loadHighScore();
}

void ScoreManager::addPoints(int points) {
    currentScore_ += points;
    
    // Check if this is a new high score
    if (currentScore_ > highScore_) {
        highScore_ = currentScore_;
        saveHighScore();
    }
}

int ScoreManager::getCurrentScore() const {
    return currentScore_;
}

int ScoreManager::getHighScore() const {
    return highScore_;
}

void ScoreManager::resetScore() {
    currentScore_ = 0;
}

void ScoreManager::loadHighScore() {
    std::ifstream file(HIGH_SCORE_FILENAME);
    if (file.is_open()) {
        std::string line;
        if (std::getline(file, line)) {
            try {
                highScore_ = std::stoi(line);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing high score: " << e.what() << std::endl;
                highScore_ = DEFAULT_HIGH_SCORE;
            }
        }
        file.close();
    } else {
        // File doesn't exist, use default
        highScore_ = DEFAULT_HIGH_SCORE;
    }
}

void ScoreManager::saveHighScore() {
    std::ofstream file(HIGH_SCORE_FILENAME);
    if (file.is_open()) {
        file << highScore_;
        file.close();
    } else {
        std::cerr << "Error saving high score to file" << std::endl;
    }
}

bool ScoreManager::isNewHighScore() const {
    return currentScore_ > highScore_;
}

std::string ScoreManager::getScoreText() const {
    std::ostringstream oss;
    oss << "Score: " << currentScore_;
    return oss.str();
}

std::string ScoreManager::getHighScoreText() const {
    std::ostringstream oss;
    oss << "High Score: " << highScore_;
    return oss.str();
}

int ScoreManager::getCheckpointScore() const {
    return checkpointScore_;
}

void ScoreManager::setCheckpointScore(int score) {
    checkpointScore_ = score;
}

void ScoreManager::initializeHighScore() {
    if (highScore_ == DEFAULT_HIGH_SCORE) {
        saveHighScore();
    }
}

} // namespace SnakeGame
