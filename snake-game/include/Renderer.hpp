#pragma once

#include "Common.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <string>

namespace SnakeGame {

class Renderer {
public:
    Renderer(int width, int height, const std::string& title = std::string("Snake Game"));
    ~Renderer();

    // Window management
    bool initialize();
    void cleanup();
    bool isInitialized() const { return initialized_; }

    // Rendering
    void clear();
    void present();
    void setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    
    // Drawing functions
    void drawRect(int x, int y, int w, int h);
    void drawFilledRect(int x, int y, int w, int h);
    void drawText(const std::string& text, int x, int y, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255);
    
    // Game-specific rendering
    void renderSnake(const SnakeBody& snakeBody, int cellSize);
    void renderFood(const Position& foodPos, int cellSize);
    void renderBoard(int boardWidth, int boardHeight, int cellSize);
    void renderScore(int score, int highScore);
    
    // Getters
    SDL_Window* getWindow() const { return window_; }
    SDL_Renderer* getRenderer() const { return renderer_; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }

private:
    SDL_Window* window_;
    SDL_Renderer* renderer_;
    TTF_Font* font_;
    int width_;
    int height_;
    bool initialized_;
    
    // Colors
    static constexpr uint8_t SNAKE_HEAD_R = 0, SNAKE_HEAD_G = 255, SNAKE_HEAD_B = 0;
    static constexpr uint8_t SNAKE_BODY_R = 0, SNAKE_BODY_G = 200, SNAKE_BODY_B = 0;
    static constexpr uint8_t FOOD_R = 255, FOOD_G = 0, FOOD_B = 0;
    static constexpr uint8_t WALL_R = 128, WALL_G = 128, WALL_B = 128;
    static constexpr uint8_t BACKGROUND_R = 0, BACKGROUND_G = 0, BACKGROUND_B = 0;
    
    bool initializeFont();
    void cleanupFont();
};

} // namespace SnakeGame
