#include "Snake.hpp"
#include <algorithm>

namespace SnakeGame {

Snake::Snake(const Position& startPos) 
    : direction_(Direction::RIGHT) {
    reset(startPos);
}

void Snake::move() {
    Position newHead = calculateNextPosition();
    
    // Add new head
    body_.insert(body_.begin(), newHead);
    
    // Remove tail (snake doesn't grow unless eating food)
    body_.pop_back();
}

void Snake::changeDirection(Direction newDirection) {
    if (canChangeDirection(newDirection)) {
        direction_ = newDirection;
    }
}

bool Snake::canChangeDirection(Direction newDirection) const {
    // Prevent 180-degree turns
    switch (direction_) {
        case Direction::UP:
            return newDirection != Direction::DOWN;
        case Direction::DOWN:
            return newDirection != Direction::UP;
        case Direction::LEFT:
            return newDirection != Direction::RIGHT;
        case Direction::RIGHT:
            return newDirection != Direction::LEFT;
        default:
            return false;
    }
}

void Snake::grow() {
    // Add a new segment at the tail
    if (!body_.empty()) {
        Position tail = body_.back();
        body_.push_back(tail);
    }
}

void Snake::reset(const Position& startPos) {
    body_.clear();
    direction_ = Direction::RIGHT;
    
    // Initialize snake with 3 segments
    body_.push_back(startPos);
    body_.push_back({startPos.first - 1, startPos.second});
    body_.push_back({startPos.first - 2, startPos.second});
}

const SnakeBody& Snake::getBody() const {
    return body_;
}

Position Snake::getHead() const {
    return body_.empty() ? Position{0, 0} : body_.front();
}

Position Snake::getTail() const {
    return body_.empty() ? Position{0, 0} : body_.back();
}

Direction Snake::getDirection() const {
    return direction_;
}

int Snake::getLength() const {
    return body_.size();
}

bool Snake::checkSelfCollision() const {
    if (body_.size() <= 1) {
        return false;
    }
    
    Position head = body_.front();
    
    // Check if head collides with any body segment
    for (size_t i = 1; i < body_.size(); ++i) {
        if (body_[i] == head) {
            return true;
        }
    }
    
    return false;
}

bool Snake::checkWallCollision(int boardWidth, int boardHeight) const {
    Position head = getHead();
    
    return head.first < 0 || head.first >= boardWidth ||
           head.second < 0 || head.second >= boardHeight;
}

bool Snake::isPositionOccupied(const Position& pos) const {
    return std::find(body_.begin(), body_.end(), pos) != body_.end();
}

void Snake::setPosition(const Position& pos) {
    if (!body_.empty()) {
        body_[0] = pos;
    }
}

Position Snake::calculateNextPosition() const {
    Position currentHead = getHead();
    
    switch (direction_) {
        case Direction::UP:
            return {currentHead.first, currentHead.second - 1};
        case Direction::DOWN:
            return {currentHead.first, currentHead.second + 1};
        case Direction::LEFT:
            return {currentHead.first - 1, currentHead.second};
        case Direction::RIGHT:
            return {currentHead.first + 1, currentHead.second};
        default:
            return currentHead;
    }
}

void Snake::updateBodyPositions() {
    // This method is called when the snake grows
    // The body segments follow the head in sequence
    for (size_t i = body_.size() - 1; i > 0; --i) {
        body_[i] = body_[i - 1];
    }
}

} // namespace SnakeGame
