/*
 * Physics Engine Patterns
 *
 * Source: PhysX, Bullet Physics, Box2D, Havok
 * Algorithm: Rigid body dynamics with collision detection and response
 *
 * What Makes It Ingenious:
 * - Broad/narrow phase collision detection for efficiency
 * - Constraint-based solving with iterative methods (PGS)
 * - Stable integration with bias and stabilization
 * - Warm starting and shock propagation for performance
 * - Island-based solving for parallelization
 * - Used in all major game engines and physics middleware
 *
 * When to Use:
 * - Rigid body simulations
 * - Collision detection and response
 * - Joint constraints (hinges, springs)
 * - Raycasting and spatial queries
 * - Vehicle physics, ragdoll systems
 * - Destructible environments
 *
 * Real-World Usage:
 * - PhysX (NVIDIA) - used in Unreal Engine, many AAA games
 * - Bullet Physics - open source, used in many games
 * - Box2D - 2D physics, used in many 2D games
 * - Havok - used in many console games
 * - Unity Physics (DOTS-based)
 * - Custom physics engines in game development
 *
 * Time Complexity:
 * - Broad phase: O(n log n) with spatial partitioning
 * - Narrow phase: O(pairs) with SAT/GJK
 * - Solver: O(constraints) with iterative methods
 * - Total: O(n + c) per frame where c is constraints
 *
 * Space Complexity: O(n + c) for bodies and constraints
 */

#include <vector>
#include <memory>
#include <unordered_set>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <functional>

// Vector2D for 2D physics (simplified)
struct Vec2 {
    float x, y;

    Vec2(float x = 0, float y = 0) : x(x), y(y) {}
    Vec2 operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }
    Vec2 operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y); }
    Vec2 operator*(float scalar) const { return Vec2(x * scalar, y * scalar); }
    Vec2 operator/(float scalar) const { return Vec2(x / scalar, y / scalar); }
    float dot(const Vec2& other) const { return x * other.x + y * other.y; }
    float cross(const Vec2& other) const { return x * other.y - y * other.x; }
    float length() const { return std::sqrt(x*x + y*y); }
    Vec2 normalized() const { float len = length(); return len > 0 ? *this / len : Vec2(0,0); }
};

// Rigid body representation
struct RigidBody {
    Vec2 position;
    Vec2 velocity;
    Vec2 force;
    float angle;
    float angular_velocity;
    float torque;
    float mass;
    float inverse_mass;
    float inertia;
    float inverse_inertia;

    // Collision shape (simplified AABB for broad phase)
    Vec2 half_extents; // Half width/height for AABB

    bool is_static;

    RigidBody(Vec2 pos = Vec2(), float mass = 1.0f, Vec2 size = Vec2(1,1))
        : position(pos), velocity(0,0), force(0,0), angle(0), angular_velocity(0),
          torque(0), mass(mass), inverse_mass(mass > 0 ? 1.0f/mass : 0),
          inertia(mass * (size.x*size.x + size.y*size.y) / 12.0f),
          inverse_inertia(inertia > 0 ? 1.0f/inertia : 0),
          half_extents(size.x/2, size.y/2), is_static(mass == 0) {}

    void ApplyForce(const Vec2& f, const Vec2& world_point = Vec2()) {
        force = force + f;
        if (!is_static && world_point.x != 0 || world_point.y != 0) {
            Vec2 r = world_point - position;
            torque += r.cross(f);
        }
    }

    void IntegrateForces(float dt) {
        if (is_static) return;

        // v += (1/m * F) * dt
        velocity = velocity + (force * inverse_mass) * dt;
        // ω += (1/I * τ) * dt
        angular_velocity += (torque * inverse_inertia) * dt;

        // Clear forces
        force = Vec2(0,0);
        torque = 0;
    }

    void IntegrateVelocity(float dt) {
        if (is_static) return;

        // x += v * dt
        position = position + velocity * dt;
        // θ += ω * dt
        angle += angular_velocity * dt;
    }

    Vec2 GetWorldPoint(const Vec2& local_point) const {
        // Rotate and translate
        float cos_a = std::cos(angle);
        float sin_a = std::sin(angle);
        return position + Vec2(
            local_point.x * cos_a - local_point.y * sin_a,
            local_point.x * sin_a + local_point.y * cos_a
        );
    }

    Vec2 GetLocalPoint(const Vec2& world_point) const {
        Vec2 r = world_point - position;
        float cos_a = std::cos(angle);
        float sin_a = std::sin(angle);
        return Vec2(
            r.x * cos_a + r.y * sin_a,
            -r.x * sin_a + r.y * cos_a
        );
    }

    // AABB for broad phase
    struct AABB {
        Vec2 min, max;
        AABB(Vec2 min, Vec2 max) : min(min), max(max) {}
        bool overlaps(const AABB& other) const {
            return !(max.x < other.min.x || min.x > other.max.x ||
                     max.y < other.min.y || min.y > other.max.y);
        }
    };

    AABB GetAABB() const {
        // Get corners and find min/max
        Vec2 corners[4] = {
            GetWorldPoint(Vec2(-half_extents.x, -half_extents.y)),
            GetWorldPoint(Vec2(half_extents.x, -half_extents.y)),
            GetWorldPoint(Vec2(-half_extents.x, half_extents.y)),
            GetWorldPoint(Vec2(half_extents.x, half_extents.y))
        };

        Vec2 min = corners[0], max = corners[0];
        for (int i = 1; i < 4; ++i) {
            min.x = std::min(min.x, corners[i].x);
            min.y = std::min(min.y, corners[i].y);
            max.x = std::max(max.x, corners[i].x);
            max.y = std::max(max.y, corners[i].y);
        }

        return AABB(min, max);
    }
};

// Collision constraint (contact point)
struct ContactConstraint {
    RigidBody* body_a;
    RigidBody* body_b;
    Vec2 world_point;      // Contact point in world space
    Vec2 normal;          // Contact normal (from A to B)
    float penetration;    // Penetration depth
    float restitution;    // Bounciness (0-1)
    float friction;       // Friction coefficient

    // Solver variables
    Vec2 relative_velocity;
    float mass_normal;    // Effective mass along normal
    float mass_tangent;   // Effective mass along tangent
    float bias;           // Baumgarte stabilization bias

    // Accumulated impulses
    float normal_impulse;
    float tangent_impulse;

    ContactConstraint(RigidBody* a, RigidBody* b, Vec2 point, Vec2 n,
                     float pen, float rest = 0.3f, float fric = 0.3f)
        : body_a(a), body_b(b), world_point(point), normal(n),
          penetration(pen), restitution(rest), friction(fric),
          mass_normal(0), mass_tangent(0), bias(0),
          normal_impulse(0), tangent_impulse(0) {}

    void PreSolve(float dt, float slop = 0.05f, float bias_factor = 0.2f) {
        // Calculate relative velocity at contact point
        Vec2 ra = world_point - body_a->position;
        Vec2 rb = world_point - body_b->position;

        Vec2 va = body_a->velocity + Vec2(-ra.y, ra.x) * body_a->angular_velocity;
        Vec2 vb = body_b->velocity + Vec2(-rb.y, rb.x) * body_b->angular_velocity;
        relative_velocity = vb - va;

        // Calculate effective mass
        float rn_a = ra.cross(normal);
        float rn_b = rb.cross(normal);
        mass_normal = body_a->inverse_mass + body_b->inverse_mass +
                     body_a->inverse_inertia * rn_a * rn_a +
                     body_b->inverse_inertia * rn_b * rn_b;
        mass_normal = mass_normal > 0 ? 1.0f / mass_normal : 0;

        // Tangent vector
        Vec2 tangent(-normal.y, normal.x);
        float rt_a = ra.cross(tangent);
        float rt_b = rb.cross(tangent);
        mass_tangent = body_a->inverse_mass + body_b->inverse_mass +
                      body_a->inverse_inertia * rt_a * rt_a +
                      body_b->inverse_inertia * rt_b * rt_b;
        mass_tangent = mass_tangent > 0 ? 1.0f / mass_tangent : 0;

        // Baumgarte stabilization
        bias = -bias_factor / dt * std::max(penetration - slop, 0.0f);

        // Warm starting (re-apply previous impulses)
        ApplyImpulses(normal_impulse, tangent_impulse);
    }

    void ApplyImpulses(float normal_imp, float tangent_imp) {
        Vec2 impulse = normal * normal_imp + Vec2(-normal.y, normal.x) * tangent_imp;

        body_a->velocity = body_a->velocity - impulse * body_a->inverse_mass;
        body_a->angular_velocity -= (world_point - body_a->position).cross(impulse) * body_a->inverse_inertia;

        body_b->velocity = body_b->velocity + impulse * body_b->inverse_mass;
        body_b->angular_velocity += (world_point - body_b->position).cross(impulse) * body_b->inverse_inertia;
    }

    void Solve(float dt) {
        // Solve normal constraint (non-penetration)
        float vn = relative_velocity.dot(normal);
        float dvn = -vn + bias;
        float delta_normal = mass_normal * dvn;
        float old_normal = normal_impulse;
        normal_impulse = std::max(normal_impulse + delta_normal, 0.0f);
        delta_normal = normal_impulse - old_normal;

        // Solve friction constraint
        float vt = relative_velocity.dot(Vec2(-normal.y, normal.x));
        float max_friction = friction * normal_impulse;
        float delta_tangent = mass_tangent * (-vt);
        float old_tangent = tangent_impulse;
        tangent_impulse = std::max(-max_friction, std::min(max_friction, tangent_impulse + delta_tangent));
        delta_tangent = tangent_impulse - old_tangent;

        // Apply impulses
        ApplyImpulses(delta_normal, delta_tangent);
    }
};

// Physics World
class PhysicsWorld {
private:
    std::vector<std::unique_ptr<RigidBody>> bodies_;
    std::vector<ContactConstraint> constraints_;
    Vec2 gravity_;

    // Broad phase: Simple AABB overlap test
    std::vector<std::pair<size_t, size_t>> BroadPhase() {
        std::vector<std::pair<size_t, size_t>> pairs;

        for (size_t i = 0; i < bodies_.size(); ++i) {
            for (size_t j = i + 1; j < bodies_.size(); ++j) {
                if (bodies_[i]->GetAABB().overlaps(bodies_[j]->GetAABB())) {
                    pairs.emplace_back(i, j);
                }
            }
        }

        return pairs;
    }

    // Narrow phase: SAT collision detection (simplified)
    void NarrowPhase(const std::vector<std::pair<size_t, size_t>>& pairs) {
        constraints_.clear();

        for (auto& pair : pairs) {
            RigidBody* body_a = bodies_[pair.first].get();
            RigidBody* body_b = bodies_[pair.second].get();

            // Skip static-static collisions
            if (body_a->is_static && body_b->is_static) continue;

            // Simple AABB vs AABB collision (can be upgraded to SAT)
            auto aabb_a = body_a->GetAABB();
            auto aabb_b = body_b->GetAABB();

            if (aabb_a.overlaps(aabb_b)) {
                // Create contact constraint at center of overlap
                Vec2 center_a = (aabb_a.min + aabb_a.max) * 0.5f;
                Vec2 center_b = (aabb_b.min + aabb_b.max) * 0.5f;
                Vec2 normal = (center_b - center_a).normalized();
                float penetration = 0.1f; // Simplified

                Vec2 contact_point = (center_a + center_b) * 0.5f;
                constraints_.emplace_back(body_a, body_b, contact_point,
                                        normal, penetration);
            }
        }
    }

public:
    PhysicsWorld(Vec2 gravity = Vec2(0, -9.81f)) : gravity_(gravity) {}

    RigidBody* CreateBody(Vec2 position, float mass, Vec2 size) {
        bodies_.push_back(std::make_unique<RigidBody>(position, mass, size));
        return bodies_.back().get();
    }

    RigidBody* CreateStaticBody(Vec2 position, Vec2 size) {
        bodies_.push_back(std::make_unique<RigidBody>(position, 0, size));
        return bodies_.back().get();
    }

    void Step(float dt, int velocity_iterations = 8, int position_iterations = 3) {
        // Apply gravity
        for (auto& body : bodies_) {
            if (!body->is_static) {
                body->ApplyForce(gravity_ * body->mass);
            }
        }

        // Integrate forces
        for (auto& body : bodies_) {
            body->IntegrateForces(dt);
        }

        // Broad phase collision detection
        auto potential_pairs = BroadPhase();

        // Narrow phase collision detection
        NarrowPhase(potential_pairs);

        // Pre-solve constraints
        for (auto& constraint : constraints_) {
            constraint.PreSolve(dt);
        }

        // Solve constraints (iterative solver)
        for (int iter = 0; iter < velocity_iterations; ++iter) {
            for (auto& constraint : constraints_) {
                constraint.Solve(dt);
            }
        }

        // Integrate velocities
        for (auto& body : bodies_) {
            body->IntegrateVelocity(dt);
        }

        // Position correction (optional)
        for (int iter = 0; iter < position_iterations; ++iter) {
            for (auto& constraint : constraints_) {
                // Position correction would go here
            }
        }

        // Clear forces for next frame
        for (auto& body : bodies_) {
            body->force = Vec2(0, 0);
            body->torque = 0;
        }
    }

    const std::vector<std::unique_ptr<RigidBody>>& GetBodies() const {
        return bodies_;
    }

    void PrintState() const {
        std::cout << "Physics World State:" << std::endl;
        for (size_t i = 0; i < bodies_.size(); ++i) {
            const auto& body = bodies_[i];
            std::cout << "Body " << i << ": pos=(" << body->position.x << ","
                      << body->position.y << ") vel=(" << body->velocity.x << ","
                      << body->velocity.y << ")" << std::endl;
        }
        std::cout << "Constraints: " << constraints_.size() << std::endl;
    }
};

// Example usage
int main() {
    std::cout << "Physics Engine Patterns Demonstration:" << std::endl;

    PhysicsWorld world(Vec2(0, -9.81f));

    // Create ground
    world.CreateStaticBody(Vec2(0, -5), Vec2(20, 1));

    // Create falling boxes
    world.CreateBody(Vec2(-2, 5), 1.0f, Vec2(1, 1));
    world.CreateBody(Vec2(0, 8), 1.0f, Vec2(1, 1));
    world.CreateBody(Vec2(2, 6), 1.0f, Vec2(1, 1));

    std::cout << "Initial state:" << std::endl;
    world.PrintState();

    // Simulate physics for a few steps
    const float dt = 1.0f / 60.0f;
    for (int step = 0; step < 10; ++step) {
        world.Step(dt);

        std::cout << "\nStep " << step + 1 << ":" << std::endl;
        world.PrintState();
    }

    std::cout << "\nPhysics simulation complete!" << std::endl;
    std::cout << "This demonstrates:" << std::endl;
    std::cout << "- Broad/narrow phase collision detection" << std::endl;
    std::cout << "- Constraint-based solver (iterative)" << std::endl;
    std::cout << "- Rigid body integration" << std::endl;
    std::cout << "- Contact resolution with friction" << std::endl;

    return 0;
}

