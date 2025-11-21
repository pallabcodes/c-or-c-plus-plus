/*
 * Recursive Particle System - Game Development
 * 
 * Source: Game particle systems (Unity, Unreal, custom engines)
 * Pattern: Recursive particle spawning and management
 * 
 * What Makes It Ingenious:
 * - Particle emitters: Recursively spawn particles
 * - Nested emitters: Particles can spawn other particles
 * - Recursive updates: Update particle hierarchies recursively
 * - Particle trails: Recursive trail generation
 * - Used in visual effects, explosions, fire, smoke, magic effects
 * 
 * When to Use:
 * - Visual effects systems
 * - Particle effects
 * - Explosion effects
 * - Fire and smoke effects
 * - Magic spell effects
 * 
 * Real-World Usage:
 * - Game engines (Unity, Unreal)
 * - Visual effects systems
 * - Particle middleware
 * - Special effects in games
 * - Environmental effects
 * 
 * Time Complexity: O(n) where n is number of particles
 * Space Complexity: O(n) for particle tree
 */

#include <vector>
#include <memory>
#include <random>
#include <cmath>
#include <iostream>
#include <functional>

class RecursiveParticleSystem {
public:
    // 3D Vector
    struct Vector3 {
        float x, y, z;
        Vector3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
        
        Vector3 operator+(const Vector3& other) const {
            return Vector3(x + other.x, y + other.y, z + other.z);
        }
        
        Vector3 operator*(float scalar) const {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }
    };
    
    // Particle
    class Particle {
    private:
        Vector3 position_;
        Vector3 velocity_;
        Vector3 acceleration_;
        float lifetime_;
        float max_lifetime_;
        float size_;
        float age_;
        bool alive_;
        std::vector<std::shared_ptr<Particle>> children_;
        std::shared_ptr<class ParticleEmitter> emitter_;
        
    public:
        Particle(Vector3 pos, Vector3 vel, float lifetime, float size)
            : position_(pos), velocity_(vel), acceleration_(0, -9.8f, 0),
              lifetime_(lifetime), max_lifetime_(lifetime), size_(size),
              age_(0.0f), alive_(true) {}
        
        void update(float delta_time) {
            if (!alive_) return;
            
            age_ += delta_time;
            
            // Update physics
            velocity_ = velocity_ + acceleration_ * delta_time;
            position_ = position_ + velocity_ * delta_time;
            
            // Check lifetime
            if (age_ >= max_lifetime_) {
                alive_ = false;
            }
            
            // Update children recursively
            for (auto& child : children_) {
                child->update(delta_time);
            }
            
            // Remove dead children
            children_.erase(
                std::remove_if(children_.begin(), children_.end(),
                    [](const std::shared_ptr<Particle>& p) {
                        return !p->is_alive();
                    }),
                children_.end()
            );
            
            // Spawn new particles from emitter
            if (emitter_ && alive_) {
                emitter_->update(delta_time, position_, children_);
            }
        }
        
        void add_child(std::shared_ptr<Particle> child) {
            children_.push_back(child);
        }
        
        void set_emitter(std::shared_ptr<class ParticleEmitter> emitter) {
            emitter_ = emitter;
        }
        
        bool is_alive() const { return alive_; }
        Vector3 get_position() const { return position_; }
        float get_age() const { return age_; }
        float get_lifetime() const { return max_lifetime_; }
        float get_size() const { return size_; }
        
        // Get all particles recursively (for rendering)
        void get_all_particles(std::vector<std::shared_ptr<Particle>>& particles) {
            if (alive_) {
                particles.push_back(shared_from_this());
            }
            for (auto& child : children_) {
                child->get_all_particles(particles);
            }
        }
    };
    
    // Particle emitter
    class ParticleEmitter {
    private:
        float spawn_rate_;
        float spawn_timer_;
        int particles_per_spawn_;
        float particle_lifetime_;
        float particle_size_;
        Vector3 velocity_range_min_;
        Vector3 velocity_range_max_;
        std::shared_ptr<ParticleEmitter> child_emitter_;
        std::mt19937 rng_;
        
    public:
        ParticleEmitter(float rate, int per_spawn, float lifetime, float size)
            : spawn_rate_(rate), spawn_timer_(0.0f), particles_per_spawn_(per_spawn),
              particle_lifetime_(lifetime), particle_size_(size),
              velocity_range_min_(-1, 0, -1), velocity_range_max_(1, 5, 1),
              rng_(std::random_device{}()) {}
        
        void set_velocity_range(Vector3 min, Vector3 max) {
            velocity_range_min_ = min;
            velocity_range_max_ = max;
        }
        
        void set_child_emitter(std::shared_ptr<ParticleEmitter> emitter) {
            child_emitter_ = emitter;
        }
        
        void update(float delta_time, Vector3 position, 
                   std::vector<std::shared_ptr<Particle>>& particles) {
            spawn_timer_ += delta_time;
            
            if (spawn_timer_ >= 1.0f / spawn_rate_) {
                spawn_timer_ = 0.0f;
                
                // Spawn particles
                for (int i = 0; i < particles_per_spawn_; i++) {
                    // Random velocity
                    std::uniform_real_distribution<float> dist_x(
                        velocity_range_min_.x, velocity_range_max_.x);
                    std::uniform_real_distribution<float> dist_y(
                        velocity_range_min_.y, velocity_range_max_.y);
                    std::uniform_real_distribution<float> dist_z(
                        velocity_range_min_.z, velocity_range_max_.z);
                    
                    Vector3 velocity(dist_x(rng_), dist_y(rng_), dist_z(rng_));
                    
                    auto particle = std::make_shared<Particle>(
                        position, velocity, particle_lifetime_, particle_size_);
                    
                    // Set child emitter if exists (recursive spawning)
                    if (child_emitter_) {
                        particle->set_emitter(child_emitter_);
                    }
                    
                    particles.push_back(particle);
                }
            }
        }
    };
    
    // Particle system manager
    class ParticleSystem {
    private:
        std::vector<std::shared_ptr<Particle>> particles_;
        std::shared_ptr<ParticleEmitter> root_emitter_;
        Vector3 position_;
        
    public:
        ParticleSystem(Vector3 pos, std::shared_ptr<ParticleEmitter> emitter)
            : position_(pos), root_emitter_(emitter) {}
        
        void update(float delta_time) {
            // Update root emitter
            if (root_emitter_) {
                root_emitter_->update(delta_time, position_, particles_);
            }
            
            // Update all particles recursively
            for (auto& particle : particles_) {
                particle->update(delta_time);
            }
            
            // Remove dead particles
            particles_.erase(
                std::remove_if(particles_.begin(), particles_.end(),
                    [](const std::shared_ptr<Particle>& p) {
                        return !p->is_alive();
                    }),
                particles_.end()
            );
        }
        
        std::vector<std::shared_ptr<Particle>> get_all_particles() {
            std::vector<std::shared_ptr<Particle>> all;
            for (auto& particle : particles_) {
                particle->get_all_particles(all);
            }
            return all;
        }
        
        size_t get_particle_count() const {
            return particles_.size();
        }
    };
};

// Example usage
int main() {
    using namespace RecursiveParticleSystem;
    
    // Create emitter for main explosion
    auto explosion_emitter = std::make_shared<ParticleEmitter>(10.0f, 5, 2.0f, 0.5f);
    explosion_emitter->set_velocity_range(Vector3(-5, 0, -5), Vector3(5, 10, 5));
    
    // Create child emitter for sparks
    auto spark_emitter = std::make_shared<ParticleEmitter>(20.0f, 2, 0.5f, 0.1f);
    spark_emitter->set_velocity_range(Vector3(-2, 0, -2), Vector3(2, 3, 2));
    
    // Set child emitter (recursive spawning)
    explosion_emitter->set_child_emitter(spark_emitter);
    
    // Create particle system
    ParticleSystem system(Vector3(0, 0, 0), explosion_emitter);
    
    // Update system
    for (int i = 0; i < 10; i++) {
        system.update(0.016f);  // ~60 FPS
        auto all_particles = system.get_all_particles();
        std::cout << "Frame " << i << ": " << all_particles.size() 
                  << " total particles" << std::endl;
    }
    
    return 0;
}

