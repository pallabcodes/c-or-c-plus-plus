#include "Renderer.hpp"
#include <iostream>
#include <sstream>

namespace SnakeGame {

Renderer::Renderer(int width, int height, const std::string& title)
    : window_(nullptr)
    , renderer_(nullptr)
    , font_(nullptr)
    , width_(width)
    , height_(height)
    , initialized_(false) {
}

Renderer::~Renderer() {
    cleanup();
}

bool Renderer::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create window
    window_ = SDL_CreateWindow(
        "Snake Game",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width_,
        height_,
        SDL_WINDOW_SHOWN
    );

    if (!window_) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create renderer
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer_) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    // Initialize font
    if (!initializeFont()) {
        std::cerr << "Failed to initialize font!" << std::endl;
        return false;
    }

    initialized_ = true;
    return true;
}

void Renderer::cleanup() {
    if (font_) {
        cleanupFont();
    }
    
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    
    TTF_Quit();
    SDL_Quit();
    
    initialized_ = false;
}

void Renderer::clear() {
    if (renderer_) {
        setDrawColor(BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
        SDL_RenderClear(renderer_);
    }
}

void Renderer::present() {
    if (renderer_) {
        SDL_RenderPresent(renderer_);
    }
}

void Renderer::setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (renderer_) {
        SDL_SetRenderDrawColor(renderer_, r, g, b, a);
    }
}

void Renderer::drawRect(int x, int y, int w, int h) {
    if (renderer_) {
        SDL_Rect rect = {x, y, w, h};
        SDL_RenderDrawRect(renderer_, &rect);
    }
}

void Renderer::drawFilledRect(int x, int y, int w, int h) {
    if (renderer_) {
        SDL_Rect rect = {x, y, w, h};
        SDL_RenderFillRect(renderer_, &rect);
    }
}

void Renderer::drawText(const std::string& text, int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    if (!font_ || !renderer_) return;
    
    SDL_Color color = {r, g, b, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font_, text.c_str(), color);
    if (!surface) return;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    if (texture) {
        SDL_Rect destRect = {x, y, surface->w, surface->h};
        SDL_RenderCopy(renderer_, texture, nullptr, &destRect);
        SDL_DestroyTexture(texture);
    }
    
    SDL_FreeSurface(surface);
}

void Renderer::renderSnake(const SnakeBody& snakeBody, int cellSize) {
    if (snakeBody.empty()) return;
    
    // Render snake head
    Position head = snakeBody.front();
    setDrawColor(SNAKE_HEAD_R, SNAKE_HEAD_G, SNAKE_HEAD_B);
    drawFilledRect(head.first * cellSize, head.second * cellSize, cellSize, cellSize);
    
    // Render snake body
    setDrawColor(SNAKE_BODY_R, SNAKE_BODY_G, SNAKE_BODY_B);
    for (size_t i = 1; i < snakeBody.size(); ++i) {
        Position body = snakeBody[i];
        drawFilledRect(body.first * cellSize, body.second * cellSize, cellSize, cellSize);
    }
}

void Renderer::renderFood(const Position& foodPos, int cellSize) {
    setDrawColor(FOOD_R, FOOD_G, FOOD_B);
    drawFilledRect(foodPos.first * cellSize, foodPos.second * cellSize, cellSize, cellSize);
}

void Renderer::renderBoard(int boardWidth, int boardHeight, int cellSize) {
    // Render walls
    setDrawColor(WALL_R, WALL_G, WALL_B);
    
    // Top and bottom walls
    for (int x = 0; x < boardWidth; ++x) {
        drawFilledRect(x * cellSize, 0, cellSize, cellSize); // Top
        drawFilledRect(x * cellSize, (boardHeight - 1) * cellSize, cellSize, cellSize); // Bottom
    }
    
    // Left and right walls
    for (int y = 0; y < boardHeight; ++y) {
        drawFilledRect(0, y * cellSize, cellSize, cellSize); // Left
        drawFilledRect((boardWidth - 1) * cellSize, y * cellSize, cellSize, cellSize); // Right
    }
}

void Renderer::renderScore(int score, int highScore) {
    std::ostringstream scoreText;
    scoreText << "Score: " << score;
    drawText(scoreText.str(), 10, 10);
    
    std::ostringstream highScoreText;
    highScoreText << "High Score: " << highScore;
    drawText(highScoreText.str(), 10, 40);
}

bool Renderer::initializeFont() {
    // Try to load a default font
    font_ = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
    if (!font_) {
        // Try alternative fonts
        font_ = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 24);
    }
    if (!font_) {
        font_ = TTF_OpenFont("/System/Library/Fonts/Arial.ttf", 24); // macOS
    }
    if (!font_) {
        font_ = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 24); // Windows
    }
    
    if (!font_) {
        std::cerr << "Warning: Could not load any font. Text rendering will be disabled." << std::endl;
        return false;
    }
    
    return true;
}

void Renderer::cleanupFont() {
    if (font_) {
        TTF_CloseFont(font_);
        font_ = nullptr;
    }
}

} // namespace SnakeGame
