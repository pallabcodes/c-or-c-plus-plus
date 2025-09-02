#include "Game.hpp"
#include <iostream>
#include <memory>
#include <SDL2/SDL.h>

int main() {
    try {
        std::cout << "Starting Snake Game with GUI..." << std::endl;
        
        // Create game configuration
        SnakeGame::GameConfig config;
        config.boardWidth = 20;
        config.boardHeight = 20;
        config.fps = 10; // Lower FPS for better control
        config.enableCheckpoints = true;
        config.enableHighScore = true;
        
        // Create the game
        auto game = std::make_unique<SnakeGame::Game>(config);
        
        std::cout << "Game initialized successfully!" << std::endl;
        
        // Initialize the game
        game->initialize();
        
        // Main game loop
        bool running = true;
        SDL_Event event;
        
        while (running) {
            // Handle events
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                        running = false;
                        break;
                    case SDL_KEYDOWN:
                        switch (event.key.keysym.sym) {
                            case SDLK_ESCAPE:
                                running = false;
                                break;
                            case SDLK_SPACE:
                                if (game->getState() == SnakeGame::GameState::MENU) {
                                    game->setState(SnakeGame::GameState::PLAYING);
                                } else if (game->getState() == SnakeGame::GameState::PLAYING) {
                                    game->setState(SnakeGame::GameState::PAUSED);
                                } else if (game->getState() == SnakeGame::GameState::PAUSED) {
                                    game->setState(SnakeGame::GameState::PLAYING);
                                } else if (game->getState() == SnakeGame::GameState::GAME_OVER) {
                                    game->restart();
                                }
                                break;
                            case SDLK_w:
                            case SDLK_UP:
                            case SDLK_s:
                            case SDLK_DOWN:
                            case SDLK_a:
                            case SDLK_LEFT:
                            case SDLK_d:
                            case SDLK_RIGHT:
                                game->handleInput(event.key.keysym.sym);
                                break;
                        }
                        break;
                }
            }
            
            // Update game logic
            if (game->getState() == SnakeGame::GameState::PLAYING) {
                game->update();
            }
            
            // Render the game
            game->render();
            
            // Control frame rate
            SDL_Delay(1000 / config.fps);
        }
        
        std::cout << "Thanks for playing Snake Game!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
    
    return 0;
}
