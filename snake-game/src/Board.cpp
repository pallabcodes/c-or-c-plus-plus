#include "Board.hpp"
#include <iostream>
#include <random>
#include <algorithm>

namespace SnakeGame {

Board::Board(int width, int height) 
    : width_(std::max(MIN_BOARD_SIZE, std::min(MAX_BOARD_SIZE, width)))
    , height_(std::max(MIN_BOARD_SIZE, std::min(MAX_BOARD_SIZE, height))) {
    grid_.resize(height_, std::vector<char>(width_, EMPTY_CELL));
}

void Board::initialize() {
    clear();
    createBoundaries();
}

void Board::clear() {
    clearGrid();
}

int Board::getWidth() const {
    return width_;
}

int Board::getHeight() const {
    return height_;
}

const std::vector<std::vector<char>>& Board::getGrid() const {
    return grid_;
}

void Board::clearGrid() {
    for (auto& row : grid_) {
        std::fill(row.begin(), row.end(), EMPTY_CELL);
    }
}

void Board::createBoundaries() {
    // Create top and bottom walls
    for (int x = 0; x < width_; ++x) {
        grid_[0][x] = WALL_CELL;                    // Top wall
        grid_[height_ - 1][x] = WALL_CELL;          // Bottom wall
    }
    
    // Create left and right walls
    for (int y = 0; y < height_; ++y) {
        grid_[y][0] = WALL_CELL;                    // Left wall
        grid_[y][width_ - 1] = WALL_CELL;           // Right wall
    }
}

bool Board::isValidPosition(const Position& pos) const {
    return pos.first >= 0 && pos.first < width_ && 
           pos.second >= 0 && pos.second < height_;
}

bool Board::isWallCollision(const Position& pos) const {
    if (!isValidPosition(pos)) {
        return true;
    }
    return grid_[pos.second][pos.first] == WALL_CELL;
}

bool Board::isSnakeCollision(const Position& pos, const SnakeBody& snakeBody) const {
    if (!isValidPosition(pos)) {
        return false; // Wall collision is handled separately
    }
    
    return std::find(snakeBody.begin(), snakeBody.end(), pos) != snakeBody.end();
}

void Board::updateGrid(const SnakeBody& snakeBody, const Position& foodPos) {
    // Clear the grid (except walls)
    for (int y = 1; y < height_ - 1; ++y) {
        for (int x = 1; x < width_ - 1; ++x) {
            grid_[y][x] = EMPTY_CELL;
        }
    }
    
    // Place snake
    if (!snakeBody.empty()) {
        // Place snake head
        Position head = snakeBody.front();
        if (isValidPosition(head)) {
            grid_[head.second][head.first] = SNAKE_HEAD;
        }
        
        // Place snake body
        for (size_t i = 1; i < snakeBody.size(); ++i) {
            Position body = snakeBody[i];
            if (isValidPosition(body)) {
                grid_[body.second][body.first] = SNAKE_BODY;
            }
        }
    }
    
    // Place food
    if (isValidPosition(foodPos)) {
        grid_[foodPos.second][foodPos.first] = FOOD_CELL;
    }
}

void Board::render() const {
    // Clear screen (platform-specific)
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    
    // Render the board
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            std::cout << grid_[y][x];
        }
        std::cout << std::endl;
    }
}

Position Board::getRandomEmptyPosition(const SnakeBody& snakeBody) const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distX(1, width_ - 2);
    std::uniform_int_distribution<> distY(1, height_ - 2);
    
    Position pos;
    int attempts = 0;
    const int maxAttempts = 100;
    
    do {
        pos = {distX(gen), distY(gen)};
        attempts++;
    } while (isPositionOccupied(pos, snakeBody) && attempts < maxAttempts);
    
    // If we couldn't find an empty position, return a default one
    if (attempts >= maxAttempts) {
        // Find first available position
        for (int y = 1; y < height_ - 1; ++y) {
            for (int x = 1; x < width_ - 1; ++x) {
                pos = {x, y};
                if (!isPositionOccupied(pos, snakeBody)) {
                    return pos;
                }
            }
        }
        // Fallback to center if everything is occupied
        pos = {width_ / 2, height_ / 2};
    }
    
    return pos;
}

bool Board::isPositionEmpty(const Position& pos, const SnakeBody& snakeBody) const {
    if (!isValidPosition(pos)) {
        return false;
    }
    
    // Check if it's a wall
    if (grid_[pos.second][pos.first] == WALL_CELL) {
        return false;
    }
    
    // Check if it's occupied by snake
    return std::find(snakeBody.begin(), snakeBody.end(), pos) == snakeBody.end();
}

bool Board::isPositionOccupied(const Position& pos, const SnakeBody& snakeBody) const {
    return !isPositionEmpty(pos, snakeBody);
}

} // namespace SnakeGame
