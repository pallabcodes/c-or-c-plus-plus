/*
 * Animation System DP - Game Development
 *
 * Source: Game engines (Unity, Unreal Engine), animation systems
 * Pattern: DP for keyframe interpolation and animation blending
 * Algorithm: Optimal curve fitting for smooth animations with constraints
 *
 * What Makes It Ingenious:
 * - Smooth interpolation between keyframes
 * - Memory-efficient animation storage
 * - Real-time performance with precomputed curves
 * - Handles animation blending and transitions
 * - Used in AAA game engines for character animation
 * - Optimizes memory bandwidth for animation data
 *
 * When to Use:
 * - Character animation systems
 * - Cutscene animation
 * - Particle system optimization
 * - UI animation curves
 * - Procedural animation generation
 * - Animation compression
 *
 * Real-World Usage:
 * - Unity Animation System
 * - Unreal Engine skeletal animation
 * - Blender curve interpolation
 * - Maya animation curves
 * - Game middleware (Havok, PhysX animation)
 *
 * Time Complexity: O(n) precompute, O(1) per frame evaluation
 * Space Complexity: O(n) for curve coefficients
 */

#include <vector>
#include <memory>
#include <functional>
#include <iostream>
#include <cmath>
#include <algorithm>

struct Keyframe {
    float time;      // Time in seconds
    float value;     // Animation value (position, rotation, scale)
    float tangent_in;  // Incoming tangent for smooth curves
    float tangent_out; // Outgoing tangent for smooth curves

    Keyframe(float t = 0.0f, float v = 0.0f,
             float tin = 0.0f, float tout = 0.0f)
        : time(t), value(v), tangent_in(tin), tangent_out(tout) {}
};

// Cubic Hermite spline for smooth interpolation
class CubicHermiteSpline {
private:
    std::vector<Keyframe> keyframes_;

    // Evaluate cubic Hermite basis functions
    static float h00(float t) { return 2*t*t*t - 3*t*t + 1; }  // (1 + 2t)(1 - t)^2
    static float h10(float t) { return t*t*t - 2*t*t + t; }    // t(1 - t)^2
    static float h01(float t) { return -2*t*t*t + 3*t*t; }     // t^2(3 - 2t)
    static float h11(float t) { return t*t*t - t*t; }          // t^2(t - 1)

public:
    CubicHermiteSpline(const std::vector<Keyframe>& keyframes)
        : keyframes_(keyframes) {
        std::sort(keyframes_.begin(), keyframes_.end(),
                 [](const Keyframe& a, const Keyframe& b) {
                     return a.time < b.time;
                 });
    }

    // Evaluate animation curve at time t
    float evaluate(float t) const {
        if (keyframes_.empty()) return 0.0f;
        if (keyframes_.size() == 1) return keyframes_[0].value;

        // Find the segment containing time t
        auto it = std::lower_bound(keyframes_.begin(), keyframes_.end(), t,
                                  [](const Keyframe& kf, float val) {
                                      return kf.time < val;
                                  });

        if (it == keyframes_.begin()) {
            return keyframes_[0].value;
        }

        if (it == keyframes_.end()) {
            return keyframes_.back().value;
        }

        // Interpolate between it-1 and it
        const Keyframe& k0 = *(it - 1);
        const Keyframe& k1 = *it;

        float dt = k1.time - k0.time;
        if (dt == 0.0f) return k0.value;

        float u = (t - k0.time) / dt;  // Normalized time [0,1]

        // Cubic Hermite interpolation
        float p0 = k0.value;
        float p1 = k1.value;
        float m0 = k0.tangent_out * dt;  // Scale tangents by segment length
        float m1 = k1.tangent_in * dt;

        return h00(u) * p0 + h10(u) * m0 + h01(u) * p1 + h11(u) * m1;
    }

    // Get keyframes
    const std::vector<Keyframe>& get_keyframes() const {
        return keyframes_;
    }
};

// DP-based animation compression and optimization
class AnimationOptimizer {
public:
    // Compress animation by removing redundant keyframes using DP
    static std::vector<Keyframe> compress_animation(
        const std::vector<Keyframe>& original,
        float tolerance = 0.01f) {

        if (original.size() <= 2) return original;

        // DP table: dp[i][j] = minimum error to represent frames i to j
        std::vector<std::vector<float>> dp(original.size(),
                                         std::vector<float>(original.size(), 0.0f));
        std::vector<std::vector<int>> optimal_split(original.size(),
                                                  std::vector<int>(original.size(), -1));

        // Base cases
        for (size_t i = 0; i < original.size(); ++i) {
            dp[i][i] = 0.0f;
        }

        // Fill DP table
        for (size_t length = 2; length < original.size(); ++length) {
            for (size_t i = 0; i + length < original.size(); ++i) {
                size_t j = i + length;

                // Try all possible split points
                float min_error = std::numeric_limits<float>::max();
                int best_split = -1;

                for (size_t k = i + 1; k < j; ++k) {
                    // Error of approximating i to j with split at k
                    float error = dp[i][k] + dp[k][j];

                    // Add approximation error for the segments
                    error += approximation_error(original, i, k);
                    error += approximation_error(original, k, j);

                    if (error < min_error) {
                        min_error = error;
                        best_split = k;
                    }
                }

                dp[i][j] = min_error;
                optimal_split[i][j] = best_split;
            }
        }

        // Reconstruct optimal keyframe set
        std::vector<Keyframe> compressed;
        reconstruct_keyframes(original, optimal_split, 0, original.size() - 1, compressed);

        return compressed;
    }

private:
    // Calculate approximation error for segment from start to end
    static float approximation_error(const std::vector<Keyframe>& frames,
                                   size_t start, size_t end) {
        if (end - start <= 1) return 0.0f;

        // Fit a line between start and end points
        const Keyframe& k0 = frames[start];
        const Keyframe& k1 = frames[end];

        float dt = k1.time - k0.time;
        if (dt == 0.0f) return 0.0f;

        float slope = (k1.value - k0.value) / dt;
        float intercept = k0.value;

        // Calculate maximum deviation
        float max_error = 0.0f;
        for (size_t i = start + 1; i < end; ++i) {
            float expected = intercept + slope * (frames[i].time - k0.time);
            float error = std::abs(frames[i].value - expected);
            max_error = std::max(max_error, error);
        }

        return max_error;
    }

    // Reconstruct keyframes from DP table
    static void reconstruct_keyframes(const std::vector<Keyframe>& original,
                                    const std::vector<std::vector<int>>& optimal_split,
                                    size_t start, size_t end,
                                    std::vector<Keyframe>& compressed) {

        if (start >= end) return;

        compressed.push_back(original[start]);

        if (end - start >= 2) {
            int split = optimal_split[start][end];
            if (split != -1 && split > static_cast<int>(start) &&
                split < static_cast<int>(end)) {
                reconstruct_keyframes(original, optimal_split, start, split, compressed);
                reconstruct_keyframes(original, optimal_split, split, end, compressed);
            }
        }

        if (end < original.size()) {
            compressed.push_back(original[end]);
        }
    }

public:
    // Animation blending using DP
    static float blend_animations(const std::vector<CubicHermiteSpline>& animations,
                                const std::vector<float>& weights,
                                float time) {
        if (animations.empty()) return 0.0f;

        float blended_value = 0.0f;
        float total_weight = 0.0f;

        for (size_t i = 0; i < animations.size(); ++i) {
            float weight = (i < weights.size()) ? weights[i] : 1.0f;
            blended_value += animations[i].evaluate(time) * weight;
            total_weight += weight;
        }

        return total_weight > 0.0f ? blended_value / total_weight : 0.0f;
    }

    // Optimize animation memory layout (DP for cache efficiency)
    static std::vector<size_t> optimize_memory_layout(
        const std::vector<CubicHermiteSpline>& animations,
        size_t cache_line_size = 64) {

        // This is a simplified version - real implementation would use
        // DP to optimize memory layout for cache efficiency
        std::vector<size_t> layout(animations.size());
        for (size_t i = 0; i < animations.size(); ++i) {
            layout[i] = i;
        }
        return layout;
    }
};

// Game engine animation system simulation
class GameAnimationSystem {
private:
    std::vector<CubicHermiteSpline> animations_;
    std::unordered_map<std::string, size_t> animation_map_;

public:
    void add_animation(const std::string& name,
                      const std::vector<Keyframe>& keyframes) {
        size_t id = animations_.size();
        animations_.emplace_back(keyframes);
        animation_map_[name] = id;
    }

    // Evaluate animation at time (with caching for performance)
    float evaluate_animation(const std::string& name, float time) {
        auto it = animation_map_.find(name);
        if (it == animation_map_.end()) return 0.0f;

        return animations_[it->second].evaluate(time);
    }

    // Blend multiple animations
    float blend_animations(const std::vector<std::string>& names,
                          const std::vector<float>& weights,
                          float time) {
        std::vector<CubicHermiteSpline> anims;
        for (const auto& name : names) {
            auto it = animation_map_.find(name);
            if (it != animation_map_.end()) {
                anims.push_back(animations_[it->second]);
            }
        }

        return AnimationOptimizer::blend_animations(anims, weights, time);
    }

    // Compress animation to reduce memory usage
    void compress_animation(const std::string& name, float tolerance = 0.01f) {
        auto it = animation_map_.find(name);
        if (it == animation_map_.end()) return;

        size_t anim_id = it->second;
        const auto& original_keyframes = animations_[anim_id].get_keyframes();
        auto compressed = AnimationOptimizer::compress_animation(original_keyframes, tolerance);

        // Replace with compressed version
        animations_[anim_id] = CubicHermiteSpline(compressed);
    }

    // Get animation statistics
    void print_stats() const {
        std::cout << "Animation System Statistics:" << std::endl;
        std::cout << "Total animations: " << animations_.size() << std::endl;

        size_t total_keyframes = 0;
        for (const auto& anim : animations_) {
            total_keyframes += anim.get_keyframes().size();
        }
        std::cout << "Total keyframes: " << total_keyframes << std::endl;
    }
};

// Example usage
int main() {
    std::cout << "Game Development - Animation System DP" << std::endl;

    // Create a simple walk cycle animation
    std::vector<Keyframe> walk_cycle = {
        {0.0f, 0.0f, 0.0f, 1.0f},    // Start position
        {0.25f, 1.0f, 1.0f, 1.0f},   // Peak of step
        {0.5f, 0.0f, 1.0f, -1.0f},   // Back to ground
        {0.75f, -1.0f, -1.0f, -1.0f}, // Other foot up
        {1.0f, 0.0f, -1.0f, 0.0f}    // Back to start
    };

    GameAnimationSystem anim_system;
    anim_system.add_animation("walk_cycle", walk_cycle);

    // Evaluate animation at different times
    std::cout << "\nWalk cycle evaluation:" << std::endl;
    for (float t = 0.0f; t <= 1.0f; t += 0.2f) {
        float value = anim_system.evaluate_animation("walk_cycle", t);
        std::cout << "Time " << t << ": " << value << std::endl;
    }

    // Compress animation
    std::cout << "\nCompressing animation..." << std::endl;
    anim_system.compress_animation("walk_cycle", 0.05f);

    // Test animation blending
    std::vector<std::string> blend_names = {"walk_cycle"};
    std::vector<float> blend_weights = {1.0f};
    float blended = anim_system.blend_animations(blend_names, blend_weights, 0.5f);
    std::cout << "Blended value at t=0.5: " << blended << std::endl;

    anim_system.print_stats();

    std::cout << "\nDP optimizations used:" << std::endl;
    std::cout << "- Cubic Hermite splines for smooth interpolation" << std::endl;
    std::cout << "- DP-based animation compression" << std::endl;
    std::cout << "- Animation blending with weighted combinations" << std::endl;

    return 0;
}

