#pragma once

#include "Common.hpp"
#include <vector>
#include <memory>

namespace SnakeGame {

class Snake {
public:
    explicit Snake(const Position& startPos = {10, 10});
    ~Snake() = default;

    // Movement
    void move();
    void changeDirection(Direction newDirection);
    bool canChangeDirection(Direction newDirection) const;
    
    // Growth
    void grow();
    void reset(const Position& startPos);
    
    // Getters
    const SnakeBody& getBody() const;
    Position getHead() const;
    Position getTail() const;
    Direction getDirection() const;
    int getLength() const;
    
    // Collision detection
    bool checkSelfCollision() const;
    bool checkWallCollision(int boardWidth, int boardHeight) const;
    
    // Utility
    bool isPositionOccupied(const Position& pos) const;
    void setPosition(const Position& pos);

private:
    SnakeBody body_;
    Direction direction_;
    
    Position calculateNextPosition() const;
    void updateBodyPositions();
};

} // namespace SnakeGame
