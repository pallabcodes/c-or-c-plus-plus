/*
 * Recursive Scene Graph - Game Development
 * 
 * Source: Game engines (Unity, Unreal, custom engines)
 * Pattern: Recursive traversal of hierarchical scene objects
 * 
 * What Makes It Ingenious:
 * - Hierarchical scene organization: Parent-child relationships
 * - Recursive transformation: Apply parent transforms to children
 * - Recursive rendering: Traverse and render scene objects
 * - Recursive culling: Cull invisible objects recursively
 * - Used in scene management, rendering, game object hierarchies
 * 
 * When to Use:
 * - Scene management systems
 * - Game object hierarchies
 * - Rendering systems
 * - Transform hierarchies
 * - UI systems
 * 
 * Real-World Usage:
 * - Game engines (Unity, Unreal Engine)
 * - 3D graphics engines
 * - Scene management systems
 * - Game object systems
 * - UI frameworks
 * 
 * Time Complexity: O(n) where n is number of scene nodes
 * Space Complexity: O(h) where h is tree height
 */

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <iostream>
#include <algorithm>

class RecursiveSceneGraph {
public:
    // Transform component
    struct Transform {
        float x, y, z;
        float rotation_x, rotation_y, rotation_z;
        float scale_x, scale_y, scale_z;
        
        Transform() : x(0), y(0), z(0), rotation_x(0), rotation_y(0), rotation_z(0),
                     scale_x(1), scale_y(1), scale_z(1) {}
        
        Transform combine(const Transform& parent) const {
            Transform result;
            // Simplified: would use proper matrix multiplication in real implementation
            result.x = parent.x + x * parent.scale_x;
            result.y = parent.y + y * parent.scale_y;
            result.z = parent.z + z * parent.scale_z;
            result.rotation_x = parent.rotation_x + rotation_x;
            result.rotation_y = parent.rotation_y + rotation_y;
            result.rotation_z = parent.rotation_z + rotation_z;
            result.scale_x = parent.scale_x * scale_x;
            result.scale_y = parent.scale_y * scale_y;
            result.scale_z = parent.scale_z * scale_z;
            return result;
        }
    };
    
    // Scene node (game object)
    class SceneNode {
    private:
        std::string name_;
        Transform local_transform_;
        Transform world_transform_;
        bool visible_;
        bool active_;
        
        std::vector<std::shared_ptr<SceneNode>> children_;
        std::weak_ptr<SceneNode> parent_;
        
    public:
        SceneNode(const std::string& name) 
            : name_(name), visible_(true), active_(true) {}
        
        void set_local_transform(const Transform& t) {
            local_transform_ = t;
        }
        
        Transform get_local_transform() const {
            return local_transform_;
        }
        
        Transform get_world_transform() const {
            return world_transform_;
        }
        
        void set_visible(bool v) { visible_ = v; }
        bool is_visible() const { return visible_; }
        
        void set_active(bool a) { active_ = a; }
        bool is_active() const { return active_; }
        
        void add_child(std::shared_ptr<SceneNode> child) {
            child->parent_ = shared_from_this();
            children_.push_back(child);
        }
        
        void remove_child(std::shared_ptr<SceneNode> child) {
            children_.erase(
                std::remove_if(children_.begin(), children_.end(),
                    [&child](const std::shared_ptr<SceneNode>& node) {
                        return node == child;
                    }),
                children_.end()
            );
        }
        
        std::vector<std::shared_ptr<SceneNode>> get_children() const {
            return children_;
        }
        
        std::shared_ptr<SceneNode> get_parent() const {
            return parent_.lock();
        }
        
        // Recursively update world transforms
        void update_world_transform(const Transform& parent_world = Transform()) {
            world_transform_ = local_transform_.combine(parent_world);
            
            // Recursively update children
            for (auto& child : children_) {
                if (child->is_active()) {
                    child->update_world_transform(world_transform_);
                }
            }
        }
        
        // Recursively render scene
        void render(std::function<void(SceneNode*)> render_func) {
            if (!is_active() || !is_visible()) {
                return;
            }
            
            // Render this node
            render_func(this);
            
            // Recursively render children
            for (auto& child : children_) {
                child->render(render_func);
            }
        }
        
        // Recursively find node by name
        std::shared_ptr<SceneNode> find_node(const std::string& name) {
            if (name_ == name) {
                return shared_from_this();
            }
            
            for (auto& child : children_) {
                auto found = child->find_node(name);
                if (found) {
                    return found;
                }
            }
            
            return nullptr;
        }
        
        // Recursively get all nodes
        void get_all_nodes(std::vector<std::shared_ptr<SceneNode>>& nodes) {
            nodes.push_back(shared_from_this());
            for (auto& child : children_) {
                child->get_all_nodes(nodes);
            }
        }
        
        // Recursively cull invisible objects
        void cull(std::function<bool(const Transform&)> cull_func) {
            if (!is_active()) {
                return;
            }
            
            // Check if this node should be culled
            if (cull_func(world_transform_)) {
                set_visible(false);
            } else {
                set_visible(true);
            }
            
            // Recursively cull children
            for (auto& child : children_) {
                child->cull(cull_func);
            }
        }
        
        std::string get_name() const { return name_; }
    };
    
    // Scene graph manager
    class SceneGraph {
    private:
        std::shared_ptr<SceneNode> root_;
        
    public:
        SceneGraph() {
            root_ = std::make_shared<SceneNode>("Root");
        }
        
        std::shared_ptr<SceneNode> get_root() const {
            return root_;
        }
        
        void update() {
            if (root_) {
                root_->update_world_transform();
            }
        }
        
        void render(std::function<void(SceneNode*)> render_func) {
            if (root_) {
                root_->render(render_func);
            }
        }
        
        std::shared_ptr<SceneNode> find_node(const std::string& name) {
            if (root_) {
                return root_->find_node(name);
            }
            return nullptr;
        }
        
        void cull(std::function<bool(const Transform&)> cull_func) {
            if (root_) {
                root_->cull(cull_func);
            }
        }
    };
};

// Example usage
int main() {
    using namespace RecursiveSceneGraph;
    
    // Create scene graph
    SceneGraph scene;
    
    // Create some objects
    auto player = std::make_shared<SceneNode>("Player");
    Transform player_transform;
    player_transform.x = 0;
    player_transform.y = 0;
    player_transform.z = 0;
    player->set_local_transform(player_transform);
    
    auto weapon = std::make_shared<SceneNode>("Weapon");
    Transform weapon_transform;
    weapon_transform.x = 0.5f;
    weapon_transform.y = 0.5f;
    weapon_transform.z = 0;
    weapon->set_local_transform(weapon_transform);
    
    auto camera = std::make_shared<SceneNode>("Camera");
    Transform camera_transform;
    camera_transform.x = 0;
    camera_transform.y = 2.0f;
    camera_transform.z = -5.0f;
    camera->set_local_transform(camera_transform);
    
    // Build hierarchy
    scene.get_root()->add_child(player);
    player->add_child(weapon);
    player->add_child(camera);
    
    // Update transforms
    scene.update();
    
    // Render
    scene.render([](SceneNode* node) {
        std::cout << "Rendering: " << node->get_name() << std::endl;
    });
    
    // Find node
    auto found = scene.find_node("Weapon");
    if (found) {
        std::cout << "Found node: " << found->get_name() << std::endl;
    }
    
    return 0;
}
