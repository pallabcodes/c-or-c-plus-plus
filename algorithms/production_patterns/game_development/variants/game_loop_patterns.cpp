/*
 * Game Loop Patterns
 *
 * Source: Unity, Unreal Engine, SDL, custom game engines
 * Algorithm: Fixed/variable timestep game loops with interpolation
 *
 * What Makes It Ingenious:
 * - Fixed timestep physics: Deterministic simulation regardless of frame rate
 * - Variable timestep rendering: Smooth visuals at any frame rate
 * - Interpolation: Smooth rendering between physics updates
 * - Frame rate independence: Game logic doesn't depend on frame rate
 * - Used in all professional game engines
 *
 * When to Use:
 * - Real-time game development
 * - Physics simulations requiring determinism
 * - Smooth rendering at variable frame rates
 * - Multiplayer games needing consistent simulation
 * - Performance-critical applications
 *
 * Real-World Usage:
 * - Unity game loop (Update/FixedUpdate)
 * - Unreal Engine tick system
 * - SDL game loops
 * - Custom engines for all major games
 * - Simulation software
 *
 * Time Complexity: O(frame_rate) for rendering, O(physics_rate) for physics
 * Space Complexity: O(1) additional space beyond game state
 */

#include <chrono>
#include <thread>
#include <iostream>
#include <functional>
#include <vector>
#include <memory>

// High-resolution timer
class GameTimer {
private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::duration<double>;

    TimePoint start_time_;
    TimePoint last_frame_time_;
    Duration accumulator_;

public:
    GameTimer() : accumulator_(0) {
        Reset();
    }

    void Reset() {
        start_time_ = Clock::now();
        last_frame_time_ = start_time_;
        accumulator_ = Duration(0);
    }

    // Get time since last frame in seconds
    double GetDeltaTime() {
        TimePoint current_time = Clock::now();
        Duration delta = current_time - last_frame_time_;
        last_frame_time_ = current_time;
        return delta.count();
    }

    // Get total time since start in seconds
    double GetTotalTime() const {
        return (Clock::now() - start_time_).count();
    }

    // Accumulate time for fixed timestep
    void AccumulateTime(double delta_time, double fixed_timestep) {
        accumulator_ += Duration(delta_time);
        // Prevent spiral of death - don't accumulate too much
        if (accumulator_.count() > 5.0 * fixed_timestep) {
            accumulator_ = Duration(5.0 * fixed_timestep);
        }
    }

    // Check if we should do a fixed update
    bool ShouldUpdateFixed(double fixed_timestep) {
        return accumulator_.count() >= fixed_timestep;
    }

    // Consume fixed timestep
    void ConsumeFixedTime(double fixed_timestep) {
        accumulator_ -= Duration(fixed_timestep);
    }

    double GetAccumulator() const {
        return accumulator_.count();
    }
};

// Game state with interpolation support
struct GameState {
    double position_x;
    double position_y;
    double velocity_x;
    double velocity_y;

    // Previous state for interpolation
    GameState prev_state;

    GameState(double x = 0, double y = 0, double vx = 0, double vy = 0)
        : position_x(x), position_y(y), velocity_x(vx), velocity_y(vy), prev_state(*this) {}

    // Update physics (fixed timestep)
    void UpdatePhysics(double delta_time) {
        // Save previous state for interpolation
        prev_state = *this;

        // Simple physics: position += velocity * delta_time
        position_x += velocity_x * delta_time;
        position_y += velocity_y * delta_time;

        // Bounce off boundaries
        if (position_x < 0 || position_x > 100) {
            velocity_x = -velocity_x;
            position_x = std::max(0.0, std::min(100.0, position_x));
        }
        if (position_y < 0 || position_y > 100) {
            velocity_y = -velocity_y;
            position_y = std::max(0.0, std::min(100.0, position_y));
        }
    }

    // Interpolate between previous and current state for smooth rendering
    GameState Interpolate(double alpha) const {
        GameState interpolated = *this;
        interpolated.position_x = prev_state.position_x +
                                 (position_x - prev_state.position_x) * alpha;
        interpolated.position_y = prev_state.position_y +
                                 (position_y - prev_state.position_y) * alpha;
        return interpolated;
    }

    void Print() const {
        std::cout << "Position: (" << position_x << ", " << position_y << ") "
                  << "Velocity: (" << velocity_x << ", " << velocity_y << ")" << std::endl;
    }
};

// Unity-style game loop with FixedUpdate and Update
class UnityStyleGameLoop {
private:
    GameTimer timer_;
    GameState game_state_;

    // Callbacks for different update types
    std::function<void(double)> on_fixed_update_;  // Physics, fixed timestep
    std::function<void(double)> on_update_;        // Game logic, variable timestep
    std::function<void()> on_render_;              // Rendering, variable rate
    std::function<void()> on_late_update_;         // After all updates

    const double FIXED_TIMESTEP = 1.0 / 60.0;  // 60 FPS physics
    const double TARGET_FRAME_RATE = 1.0 / 60.0;  // Target 60 FPS rendering

    bool running_;
    int frame_count_;
    double total_time_;

public:
    UnityStyleGameLoop()
        : game_state_(50, 50, 20, 15), running_(true), frame_count_(0), total_time_(0) {

        // Set up default callbacks
        on_fixed_update_ = [this](double dt) {
            game_state_.UpdatePhysics(dt);
        };

        on_update_ = [this](double dt) {
            // Variable timestep logic here
        };

        on_render_ = [this]() {
            // Interpolate for smooth rendering
            double alpha = timer_.GetAccumulator() / FIXED_TIMESTEP;
            GameState render_state = game_state_.Interpolate(alpha);

            std::cout << "Frame " << frame_count_ << " (alpha=" << alpha << "): ";
            render_state.Print();
        };

        on_late_update_ = [this]() {
            // Cleanup, etc.
        };
    }

    void SetFixedUpdateCallback(std::function<void(double)> callback) {
        on_fixed_update_ = callback;
    }

    void SetUpdateCallback(std::function<void(double)> callback) {
        on_update_ = callback;
    }

    void SetRenderCallback(std::function<void()> callback) {
        on_render_ = callback;
    }

    void SetLateUpdateCallback(std::function<void()> callback) {
        on_late_update_ = callback;
    }

    void Run() {
        timer_.Reset();

        while (running_ && frame_count_ < 10) {  // Run for 10 frames
            double delta_time = timer_.GetDeltaTime();
            total_time_ += delta_time;

            // Accumulate time for fixed updates
            timer_.AccumulateTime(delta_time, FIXED_TIMESTEP);

            // Fixed timestep updates (physics)
            while (timer_.ShouldUpdateFixed(FIXED_TIMESTEP)) {
                on_fixed_update_(FIXED_TIMESTEP);
                timer_.ConsumeFixedTime(FIXED_TIMESTEP);
            }

            // Variable timestep update (game logic)
            on_update_(delta_time);

            // Late update
            on_late_update_();

            // Render (as fast as possible, but we throttle for demo)
            on_render_();

            frame_count_++;

            // Sleep to simulate frame rate limiting
            std::this_thread::sleep_for(std::chrono::milliseconds(16));  // ~60 FPS
        }
    }

    void Stop() {
        running_ = false;
    }

    const GameState& GetGameState() const {
        return game_state_;
    }
};

// SDL-style game loop (simpler, no fixed timestep)
class SDLStyleGameLoop {
private:
    GameTimer timer_;
    GameState game_state_;

    std::function<void(double)> on_update_;
    std::function<void()> on_render_;
    std::function<void()> on_event_;

    bool running_;
    int frame_count_;

public:
    SDLStyleGameLoop()
        : game_state_(50, 50, 10, 8), running_(true), frame_count_(0) {

        on_update_ = [this](double dt) {
            game_state_.UpdatePhysics(dt);
        };

        on_render_ = [this]() {
            std::cout << "SDL Frame " << frame_count_ << ": ";
            game_state_.Print();
        };

        on_event_ = []() {
            // Handle input events
        };
    }

    void SetUpdateCallback(std::function<void(double)> callback) {
        on_update_ = callback;
    }

    void SetRenderCallback(std::function<void()> callback) {
        on_render_ = callback;
    }

    void SetEventCallback(std::function<void()> callback) {
        on_event_ = callback;
    }

    void Run() {
        timer_.Reset();

        while (running_ && frame_count_ < 10) {
            double delta_time = timer_.GetDeltaTime();

            // Handle events
            on_event_();

            // Update game logic
            on_update_(delta_time);

            // Render
            on_render_();

            frame_count_++;

            // Frame rate limiting
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }

    void Stop() {
        running_ = false;
    }
};

// Advanced game loop with frame rate independence and statistics
class AdvancedGameLoop {
private:
    GameTimer timer_;
    GameState game_state_;

    double fixed_timestep_;
    double target_frame_rate_;
    bool vsync_enabled_;

    // Statistics
    int frame_count_;
    double total_time_;
    double min_frame_time_;
    double max_frame_time_;
    double avg_frame_time_;

    // Callbacks
    std::function<void(double)> on_fixed_update_;
    std::function<void(double)> on_variable_update_;
    std::function<void(double)> on_render_;
    std::function<void()> on_event_;

    bool running_;

public:
    AdvancedGameLoop(double fixed_ts = 1.0/60.0, double target_fps = 60.0, bool vsync = true)
        : fixed_timestep_(fixed_ts), target_frame_rate_(1.0/target_fps),
          vsync_enabled_(vsync), frame_count_(0), total_time_(0),
          min_frame_time_(999999), max_frame_time_(0), avg_frame_time_(0), running_(true) {

        // Initialize callbacks
        on_fixed_update_ = [this](double dt) {
            game_state_.UpdatePhysics(dt);
        };

        on_variable_update_ = [this](double dt) {
            // Variable rate logic
        };

        on_render_ = [this](double alpha) {
            GameState render_state = game_state_.Interpolate(alpha);
            std::cout << "Advanced Frame " << frame_count_ << " (alpha=" << alpha << "): ";
            render_state.Print();
        };

        on_event_ = []() {
            // Event handling
        };
    }

    void Run() {
        timer_.Reset();
        double next_frame_time = timer_.GetTotalTime();

        while (running_ && frame_count_ < 10) {
            double current_time = timer_.GetTotalTime();
            double delta_time = timer_.GetDeltaTime();

            // Update statistics
            UpdateFrameStatistics(delta_time);

            // Handle events
            on_event_();

            // Accumulate time for fixed updates
            timer_.AccumulateTime(delta_time, fixed_timestep_);

            // Fixed timestep updates
            int fixed_updates = 0;
            while (timer_.ShouldUpdateFixed(fixed_timestep_) && fixed_updates < 5) {
                on_fixed_update_(fixed_timestep_);
                timer_.ConsumeFixedTime(fixed_timestep_);
                fixed_updates++;
            }

            // Variable timestep update
            on_variable_update_(delta_time);

            // Render with interpolation
            double alpha = timer_.GetAccumulator() / fixed_timestep_;
            on_render_(alpha);

            frame_count_++;
            total_time_ = current_time;

            // Frame rate limiting
            if (vsync_enabled_) {
                next_frame_time += target_frame_rate_;
                double sleep_time = next_frame_time - timer_.GetTotalTime();
                if (sleep_time > 0) {
                    std::this_thread::sleep_for(
                        std::chrono::duration<double>(sleep_time));
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }
        }

        PrintStatistics();
    }

    void UpdateFrameStatistics(double delta_time) {
        min_frame_time_ = std::min(min_frame_time_, delta_time);
        max_frame_time_ = std::max(max_frame_time_, delta_time);
        avg_frame_time_ = (avg_frame_time_ * frame_count_ + delta_time) / (frame_count_ + 1);
    }

    void PrintStatistics() const {
        std::cout << "\nGame Loop Statistics:" << std::endl;
        std::cout << "  Frames rendered: " << frame_count_ << std::endl;
        std::cout << "  Total time: " << total_time_ << " seconds" << std::endl;
        std::cout << "  Average FPS: " << frame_count_ / total_time_ << std::endl;
        std::cout << "  Frame time - Min: " << min_frame_time_*1000 << "ms, "
                  << "Max: " << max_frame_time_*1000 << "ms, "
                  << "Avg: " << avg_frame_time_*1000 << "ms" << std::endl;
    }

    void Stop() {
        running_ = false;
    }

    void SetCallbacks(std::function<void(double)> fixed_update,
                     std::function<void(double)> var_update,
                     std::function<void(double)> render,
                     std::function<void()> event) {
        on_fixed_update_ = fixed_update;
        on_variable_update_ = var_update;
        on_render_ = render;
        on_event_ = event;
    }
};

// Example usage
int main() {
    std::cout << "Game Loop Patterns Demonstration:" << std::endl;

    std::cout << "\n1. Unity-Style Game Loop (Fixed + Variable Timestep):" << std::endl;
    UnityStyleGameLoop unity_loop;
    unity_loop.Run();

    std::cout << "\n2. SDL-Style Game Loop (Simple Variable Timestep):" << std::endl;
    SDLStyleGameLoop sdl_loop;
    sdl_loop.Run();

    std::cout << "\n3. Advanced Game Loop (With Statistics):" << std::endl;
    AdvancedGameLoop advanced_loop;
    advanced_loop.Run();

    return 0;
}

