#pragma once

#include "Common.hpp"
#include <memory>

namespace SnakeGame {

class Board;

class Food {
public:
    explicit Food(const Position& pos = {0, 0});
    ~Food() = default;

    // Food management
    void generateNewPosition(const Board& board, const SnakeBody& snakeBody);
    void setPosition(const Position& pos);
    const Position& getPosition() const;
    
    // Collision detection
    bool isEaten(const Position& snakeHead) const;
    
    // Utility
    bool isValidPosition(const Position& pos, const Board& board, const SnakeBody& snakeBody) const;

private:
    Position position_;
    
    Position findRandomEmptyPosition(const Board& board, const SnakeBody& snakeBody) const;
};

} // namespace SnakeGame
