/*
 * Recursive Animation Blending - Game Development
 * 
 * Source: Game animation systems (Unity, Unreal, animation engines)
 * Pattern: Recursive blending of animation layers and states
 * 
 * What Makes It Ingenious:
 * - Layered animation: Blend multiple animation layers recursively
 * - Additive blending: Add animations on top of base animations
 * - Recursive interpolation: Blend between animation states recursively
 * - Animation trees: Hierarchical animation blending
 * - Used in character animation, procedural animation, animation systems
 * 
 * When to Use:
 * - Character animation systems
 * - Animation state machines
 * - Procedural animation
 * - Animation layering
 * - Smooth animation transitions
 * 
 * Real-World Usage:
 * - Game engines (Unity, Unreal)
 * - Character animation systems
 * - Animation middleware
 * - Motion capture systems
 * - Procedural animation
 * 
 * Time Complexity: O(n) where n is number of animation layers
 * Space Complexity: O(n) for animation tree
 */

#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <cmath>
#include <iostream>

class RecursiveAnimationBlending {
public:
    // Animation frame data
    struct AnimationFrame {
        float time;
        std::vector<float> bone_rotations;  // Simplified: just rotations
        std::vector<float> bone_positions;
        
        AnimationFrame(int bone_count = 0) {
            bone_rotations.resize(bone_count, 0.0f);
            bone_positions.resize(bone_count, 0.0f);
        }
    };
    
    // Animation clip
    class AnimationClip {
    private:
        std::string name_;
        float duration_;
        std::vector<AnimationFrame> frames_;
        int bone_count_;
        
    public:
        AnimationClip(const std::string& name, float duration, int bone_count)
            : name_(name), duration_(duration), bone_count_(bone_count) {
            // Create keyframes (simplified)
            int frame_count = static_cast<int>(duration * 30.0f);  // 30 FPS
            for (int i = 0; i < frame_count; i++) {
                AnimationFrame frame(bone_count);
                frame.time = static_cast<float>(i) / 30.0f;
                frames_.push_back(frame);
            }
        }
        
        AnimationFrame sample(float time) const {
            time = std::fmod(time, duration_);
            
            // Find surrounding frames
            int frame_index = static_cast<int>(time * 30.0f);
            frame_index = std::min(frame_index, static_cast<int>(frames_.size() - 1));
            
            return frames_[frame_index];
        }
        
        float get_duration() const { return duration_; }
        std::string get_name() const { return name_; }
    };
    
    // Animation layer
    class AnimationLayer {
    private:
        std::string name_;
        std::shared_ptr<AnimationClip> clip_;
        float weight_;
        float time_;
        bool additive_;
        
    public:
        AnimationLayer(const std::string& name, 
                      std::shared_ptr<AnimationClip> clip,
                      float weight = 1.0f,
                      bool additive = false)
            : name_(name), clip_(clip), weight_(weight), time_(0.0f), additive_(additive) {}
        
        void update(float delta_time) {
            if (clip_) {
                time_ += delta_time;
                if (time_ > clip_->get_duration()) {
                    time_ = std::fmod(time_, clip_->get_duration());
                }
            }
        }
        
        AnimationFrame sample() const {
            if (clip_) {
                return clip_->sample(time_);
            }
            return AnimationFrame();
        }
        
        float get_weight() const { return weight_; }
        void set_weight(float w) { weight_ = std::max(0.0f, std::min(1.0f, w)); }
        bool is_additive() const { return additive_; }
        std::string get_name() const { return name_; }
    };
    
    // Animation blend node
    class BlendNode {
    private:
        std::string name_;
        std::vector<std::shared_ptr<AnimationLayer>> layers_;
        std::vector<std::shared_ptr<BlendNode>> children_;
        float blend_weight_;
        
    public:
        BlendNode(const std::string& name, float blend_weight = 1.0f)
            : name_(name), blend_weight_(blend_weight) {}
        
        void add_layer(std::shared_ptr<AnimationLayer> layer) {
            layers_.push_back(layer);
        }
        
        void add_child(std::shared_ptr<BlendNode> child) {
            children_.push_back(child);
        }
        
        // Recursively blend animations
        AnimationFrame blend(float delta_time, int bone_count) {
            // Update layers
            for (auto& layer : layers_) {
                layer->update(delta_time);
            }
            
            // Blend layers
            AnimationFrame result(bone_count);
            
            if (!layers_.empty()) {
                // Start with first layer
                result = layers_[0]->sample();
                float total_weight = layers_[0]->get_weight();
                
                // Blend remaining layers
                for (size_t i = 1; i < layers_.size(); i++) {
                    auto layer = layers_[i];
                    auto frame = layer->sample();
                    float weight = layer->get_weight();
                    
                    if (layer->is_additive()) {
                        // Additive blending
                        for (size_t j = 0; j < result.bone_rotations.size(); j++) {
                            result.bone_rotations[j] += frame.bone_rotations[j] * weight;
                        }
                    } else {
                        // Normalized blending
                        float blend_factor = weight / (total_weight + weight);
                        for (size_t j = 0; j < result.bone_rotations.size(); j++) {
                            result.bone_rotations[j] = 
                                result.bone_rotations[j] * (1.0f - blend_factor) +
                                frame.bone_rotations[j] * blend_factor;
                        }
                        total_weight += weight;
                    }
                }
            }
            
            // Recursively blend children
            if (!children_.empty()) {
                AnimationFrame child_result = children_[0]->blend(delta_time, bone_count);
                
                // Blend with child results
                for (size_t i = 1; i < children_.size(); i++) {
                    auto child_frame = children_[i]->blend(delta_time, bone_count);
                    float child_weight = children_[i]->blend_weight_;
                    
                    // Blend child results
                    for (size_t j = 0; j < result.bone_rotations.size(); j++) {
                        result.bone_rotations[j] = 
                            result.bone_rotations[j] * (1.0f - child_weight) +
                            child_frame.bone_rotations[j] * child_weight;
                    }
                }
            }
            
            // Apply node weight
            for (auto& rot : result.bone_rotations) {
                rot *= blend_weight_;
            }
            
            return result;
        }
        
        std::string get_name() const { return name_; }
    };
    
    // Animation blend tree
    class AnimationBlendTree {
    private:
        std::shared_ptr<BlendNode> root_;
        int bone_count_;
        
    public:
        AnimationBlendTree(std::shared_ptr<BlendNode> root, int bone_count)
            : root_(root), bone_count_(bone_count) {}
        
        AnimationFrame update(float delta_time) {
            if (root_) {
                return root_->blend(delta_time, bone_count_);
            }
            return AnimationFrame(bone_count_);
        }
    };
};

// Example usage
int main() {
    using namespace RecursiveAnimationBlending;
    
    // Create animation clips
    auto idle_clip = std::make_shared<AnimationClip>("Idle", 2.0f, 20);
    auto walk_clip = std::make_shared<AnimationClip>("Walk", 1.0f, 20);
    auto run_clip = std::make_shared<AnimationClip>("Run", 0.8f, 20);
    
    // Create layers
    auto idle_layer = std::make_shared<AnimationLayer>("IdleLayer", idle_clip, 1.0f);
    auto walk_layer = std::make_shared<AnimationLayer>("WalkLayer", walk_clip, 0.5f);
    auto run_layer = std::make_shared<AnimationLayer>("RunLayer", run_clip, 0.3f, true);
    
    // Create blend nodes
    auto base_node = std::make_shared<BlendNode>("Base");
    base_node->add_layer(idle_layer);
    base_node->add_layer(walk_layer);
    
    auto additive_node = std::make_shared<BlendNode>("Additive", 0.5f);
    additive_node->add_layer(run_layer);
    
    // Create root
    auto root = std::make_shared<BlendNode>("Root");
    root->add_child(base_node);
    root->add_child(additive_node);
    
    // Create blend tree
    AnimationBlendTree tree(root, 20);
    
    // Update animation
    auto frame = tree.update(0.016f);  // ~60 FPS
    std::cout << "Blended animation frame with " << frame.bone_rotations.size() 
              << " bones" << std::endl;
    
    return 0;
}

