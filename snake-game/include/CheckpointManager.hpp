#pragma once

#include "Common.hpp"
#include <vector>
#include <string>
#include <memory>

namespace SnakeGame {

class Snake;
class Food;
class ScoreManager;

class CheckpointManager {
public:
    CheckpointManager();
    ~CheckpointManager() = default;

    // Checkpoint creation
    bool shouldCreateCheckpoint(int currentScore) const;
    void createCheckpoint(const Snake& snake, const Food& food, const ScoreManager& scoreManager, GameState gameState);
    
    // Checkpoint management
    bool hasCheckpoints() const;
    int getCheckpointCount() const;
    const std::vector<CheckpointData>& getCheckpoints() const;
    
    // Checkpoint restoration
    bool restoreFromCheckpoint(size_t index, Snake& snake, Food& food, ScoreManager& scoreManager, GameState& gameState);
    bool restoreFromLastCheckpoint(Snake& snake, Food& food, ScoreManager& scoreManager, GameState& gameState);
    
    // Persistence
    void saveCheckpoints();
    void loadCheckpoints();
    void clearCheckpoints();
    
    // Utility
    std::string getCheckpointInfo(size_t index) const;
    void removeOldCheckpoints();

private:
    std::vector<CheckpointData> checkpoints_;
    std::string checkpointFile_;
    
    static constexpr const char* CHECKPOINT_FILENAME = "checkpoints.dat";
    
    void serializeCheckpoint(const CheckpointData& data, std::ofstream& file);
    CheckpointData deserializeCheckpoint(std::ifstream& file);
    bool isValidCheckpoint(const CheckpointData& data) const;
};

} // namespace SnakeGame
