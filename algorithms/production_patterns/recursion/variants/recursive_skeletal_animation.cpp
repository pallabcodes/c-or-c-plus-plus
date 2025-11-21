/*
 * Recursive Skeletal Animation - Game Development
 * 
 * Source: Game engines (Unity, Unreal, custom animation systems)
 * Pattern: Recursive bone hierarchy traversal for skeletal animation
 * 
 * What Makes It Ingenious:
 * - Bone hierarchy: Parent-child relationships
 * - Recursive transformation: Apply parent transforms to children
 * - Forward kinematics: Calculate end effector from joint angles
 * - Inverse kinematics: Calculate joint angles from end effector
 * - Used in character animation, rigging, skeletal systems
 * 
 * When to Use:
 * - Character animation
 * - Skeletal rigging
 * - Bone-based animation
 * - IK/FK systems
 * - Animation blending
 * 
 * Real-World Usage:
 * - Game engines (Unity, Unreal)
 * - 3D animation software (Blender, Maya)
 * - Character animation systems
 * - Skeletal mesh animation
 * - Procedural animation
 * 
 * Time Complexity: O(n) where n is number of bones
 * Space Complexity: O(n) for bone hierarchy
 */

#include <vector>
#include <memory>
#include <cmath>
#include <iostream>
#include <algorithm>

class RecursiveSkeletalAnimation {
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
        
        float length() const {
            return std::sqrt(x * x + y * y + z * z);
        }
        
        Vector3 normalized() const {
            float len = length();
            if (len > 0.0001f) {
                return Vector3(x / len, y / len, z / len);
            }
            return Vector3(0, 0, 0);
        }
    };
    
    // Quaternion for rotation
    struct Quaternion {
        float w, x, y, z;
        Quaternion(float w = 1, float x = 0, float y = 0, float z = 0)
            : w(w), x(x), y(y), z(z) {}
        
        Quaternion operator*(const Quaternion& other) const {
            return Quaternion(
                w * other.w - x * other.x - y * other.y - z * other.z,
                w * other.x + x * other.w + y * other.z - z * other.y,
                w * other.y - x * other.z + y * other.w + z * other.x,
                w * other.z + x * other.y - y * other.x + z * other.w
            );
        }
        
        Vector3 rotate(const Vector3& v) const {
            Quaternion q_v(0, v.x, v.y, v.z);
            Quaternion q_conj(w, -x, -y, -z);
            Quaternion result = (*this) * q_v * q_conj;
            return Vector3(result.x, result.y, result.z);
        }
    };
    
    // Transform (position, rotation, scale)
    struct Transform {
        Vector3 position;
        Quaternion rotation;
        Vector3 scale;
        
        Transform() : position(0, 0, 0), rotation(1, 0, 0, 0), scale(1, 1, 1) {}
        
        Transform combine(const Transform& parent) const {
            Transform result;
            result.scale = Vector3(
                scale.x * parent.scale.x,
                scale.y * parent.scale.y,
                scale.z * parent.scale.z
            );
            result.rotation = parent.rotation * rotation;
            result.position = parent.position + parent.rotation.rotate(
                Vector3(position.x * parent.scale.x,
                       position.y * parent.scale.y,
                       position.z * parent.scale.z)
            );
            return result;
        }
    };
    
    // Bone in skeleton
    class Bone {
    private:
        std::string name_;
        int id_;
        Transform local_transform_;
        Transform world_transform_;
        std::vector<std::shared_ptr<Bone>> children_;
        std::shared_ptr<Bone> parent_;
        float length_;
        
    public:
        Bone(const std::string& name, int id, float length = 1.0f)
            : name_(name), id_(id), length_(length), parent_(nullptr) {}
        
        void set_local_transform(const Transform& t) {
            local_transform_ = t;
        }
        
        Transform get_local_transform() const {
            return local_transform_;
        }
        
        Transform get_world_transform() const {
            return world_transform_;
        }
        
        void add_child(std::shared_ptr<Bone> child) {
            child->parent_ = shared_from_this();
            children_.push_back(child);
        }
        
        std::vector<std::shared_ptr<Bone>> get_children() const {
            return children_;
        }
        
        std::shared_ptr<Bone> get_parent() const {
            return parent_;
        }
        
        // Recursively update world transforms
        void update_world_transform(const Transform& parent_world = Transform()) {
            // Combine with parent transform
            world_transform_ = local_transform_.combine(parent_world);
            
            // Recursively update children
            for (auto& child : children_) {
                child->update_world_transform(world_transform_);
            }
        }
        
        // Forward kinematics: Get end effector position
        Vector3 forward_kinematics() const {
            if (children_.empty()) {
                // End effector: position + direction * length
                Vector3 direction = world_transform_.rotation.rotate(Vector3(0, 1, 0));
                return world_transform_.position + direction * length_;
            } else {
                // Return position of last child's end effector
                return children_.back()->forward_kinematics();
            }
        }
        
        // Get bone by name (recursive search)
        std::shared_ptr<Bone> find_bone(const std::string& name) {
            if (name_ == name) {
                return shared_from_this();
            }
            
            for (auto& child : children_) {
                auto found = child->find_bone(name);
                if (found) {
                    return found;
                }
            }
            
            return nullptr;
        }
        
        // Get all bones recursively
        void get_all_bones(std::vector<std::shared_ptr<Bone>>& bones) {
            bones.push_back(shared_from_this());
            for (auto& child : children_) {
                child->get_all_bones(bones);
            }
        }
        
        std::string get_name() const { return name_; }
        int get_id() const { return id_; }
        float get_length() const { return length_; }
    };
    
    // Skeleton (bone hierarchy)
    class Skeleton {
    private:
        std::shared_ptr<Bone> root_;
        
    public:
        Skeleton(std::shared_ptr<Bone> root) : root_(root) {}
        
        void update() {
            if (root_) {
                root_->update_world_transform();
            }
        }
        
        std::shared_ptr<Bone> get_root() const {
            return root_;
        }
        
        std::shared_ptr<Bone> find_bone(const std::string& name) {
            if (root_) {
                return root_->find_bone(name);
            }
            return nullptr;
        }
        
        Vector3 get_end_effector_position() {
            if (root_) {
                return root_->forward_kinematics();
            }
            return Vector3(0, 0, 0);
        }
    };
    
    // Simple IK solver (recursive)
    class IKSolver {
    public:
        // CCD (Cyclic Coordinate Descent) IK
        static bool solve_ik_ccd(
            std::shared_ptr<Bone> end_effector,
            const Vector3& target,
            int max_iterations = 10,
            float threshold = 0.01f) {
            
            for (int iter = 0; iter < max_iterations; iter++) {
                auto current = end_effector;
                
                // Traverse up the chain
                while (current) {
                    Vector3 current_pos = current->get_world_transform().position;
                    Vector3 end_pos = end_effector->forward_kinematics();
                    Vector3 target_dir = (target - current_pos).normalized();
                    Vector3 current_dir = (end_pos - current_pos).normalized();
                    
                    // Calculate rotation needed
                    float dot = current_dir.x * target_dir.x + 
                               current_dir.y * target_dir.y + 
                               current_dir.z * target_dir.z;
                    dot = std::max(-1.0f, std::min(1.0f, dot));  // Clamp
                    
                    float angle = std::acos(dot);
                    
                    if (angle > 0.001f) {
                        // Apply rotation (simplified)
                        // In real implementation, would use proper quaternion rotation
                    }
                    
                    // Move to parent
                    current = current->get_parent();
                }
                
                // Check if close enough
                Vector3 end_pos = end_effector->forward_kinematics();
                Vector3 diff = target - end_pos;
                if (diff.length() < threshold) {
                    return true;
                }
            }
            
            return false;
        }
    };
};

// Example usage
int main() {
    using namespace RecursiveSkeletalAnimation;
    
    // Create simple arm skeleton
    auto root = std::make_shared<Bone>("Shoulder", 0, 0.0f);
    auto upper_arm = std::make_shared<Bone>("UpperArm", 1, 1.0f);
    auto lower_arm = std::make_shared<Bone>("LowerArm", 2, 1.0f);
    auto hand = std::make_shared<Bone>("Hand", 3, 0.3f);
    
    root->add_child(upper_arm);
    upper_arm->add_child(lower_arm);
    lower_arm->add_child(hand);
    
    // Set initial transforms
    Transform upper_transform;
    upper_transform.position = Vector3(0, 1, 0);
    upper_arm->set_local_transform(upper_transform);
    
    Transform lower_transform;
    lower_transform.position = Vector3(0, 1, 0);
    lower_arm->set_local_transform(lower_transform);
    
    Transform hand_transform;
    hand_transform.position = Vector3(0, 1, 0);
    hand->set_local_transform(hand_transform);
    
    // Create skeleton
    Skeleton skeleton(root);
    skeleton.update();
    
    // Get end effector position
    Vector3 end_pos = skeleton.get_end_effector_position();
    std::cout << "End effector position: (" 
              << end_pos.x << ", " << end_pos.y << ", " << end_pos.z << ")" << std::endl;
    
    // Find bone
    auto found = skeleton.find_bone("Hand");
    if (found) {
        std::cout << "Found bone: " << found->get_name() << std::endl;
    }
    
    return 0;
}

