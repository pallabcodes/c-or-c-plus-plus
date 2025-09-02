#pragma once

#include "Common.hpp"
#include <string>
#include <fstream>

namespace SnakeGame {

class ScoreManager {
public:
    ScoreManager();
    ~ScoreManager() = default;

    // Scoring
    void addPoints(int points);
    void resetScore();
    int getCurrentScore() const;
    int getHighScore() const;
    
    // High score management
    void loadHighScore();
    void saveHighScore();
    bool isNewHighScore() const;
    
    // Score display
    std::string getScoreText() const;
    std::string getHighScoreText() const;
    
    // Checkpoint integration
    int getCheckpointScore() const;
    void setCheckpointScore(int score);

private:
    int currentScore_;
    int highScore_;
    int checkpointScore_;
    std::string highScoreFile_;
    
    static constexpr int DEFAULT_HIGH_SCORE = 0;
    static constexpr const char* HIGH_SCORE_FILENAME = "highscore.txt";
    
    void initializeHighScore();
};

} // namespace SnakeGame
