#include "Food.hpp"
#include "Board.hpp"
#include <random>
#include <algorithm>

namespace SnakeGame {

Food::Food(const Position& pos) : position_(pos) {
}

void Food::generateNewPosition(const Board& board, const SnakeBody& snakeBody) {
    position_ = findRandomEmptyPosition(board, snakeBody);
}

void Food::setPosition(const Position& pos) {
    position_ = pos;
}

const Position& Food::getPosition() const {
    return position_;
}

bool Food::isEaten(const Position& snakeHead) const {
    return position_ == snakeHead;
}

bool Food::isValidPosition(const Position& pos, const Board& board, const SnakeBody& snakeBody) const {
    // Check if position is within board bounds
    if (!board.isValidPosition(pos)) {
        return false;
    }
    
    // Check if position is not a wall
    if (board.isWallCollision(pos)) {
        return false;
    }
    
    // Check if position is not occupied by snake
    if (std::find(snakeBody.begin(), snakeBody.end(), pos) != snakeBody.end()) {
        return false;
    }
    
    return true;
}

Position Food::findRandomEmptyPosition(const Board& board, const SnakeBody& snakeBody) const {
    return board.getRandomEmptyPosition(snakeBody);
}

} // namespace SnakeGame
