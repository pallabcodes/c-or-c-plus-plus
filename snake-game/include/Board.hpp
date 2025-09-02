#pragma once

#include "Common.hpp"
#include <vector>
#include <memory>

namespace SnakeGame {

class Board {
public:
    explicit Board(int width = DEFAULT_BOARD_WIDTH, int height = DEFAULT_BOARD_HEIGHT);
    ~Board() = default;

    // Board management
    void initialize();
    void clear();
    bool isValidPosition(const Position& pos) const;
    bool isWallCollision(const Position& pos) const;
    bool isSnakeCollision(const Position& pos, const SnakeBody& snakeBody) const;
    
    // Getters
    int getWidth() const;
    int getHeight() const;
    const std::vector<std::vector<char>>& getGrid() const;
    
    // Rendering
    void render() const;
    void updateGrid(const SnakeBody& snakeBody, const Position& foodPos);
    
    // Utility
    Position getRandomEmptyPosition(const SnakeBody& snakeBody) const;
    bool isPositionEmpty(const Position& pos, const SnakeBody& snakeBody) const;
    bool isPositionOccupied(const Position& pos, const SnakeBody& snakeBody) const;

private:
    int width_;
    int height_;
    std::vector<std::vector<char>> grid_;
    
    // Grid characters
    static constexpr char EMPTY_CELL = ' ';
    static constexpr char WALL_CELL = '#';
    static constexpr char SNAKE_HEAD = 'O';
    static constexpr char SNAKE_BODY = 'o';
    static constexpr char FOOD_CELL = '*';
    
    void createBoundaries();
    void clearGrid();
};

} // namespace SnakeGame
