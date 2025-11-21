/*
 * Recursive LOD (Level of Detail) System - Game Development
 * 
 * Source: Game engines (Unity, Unreal, terrain systems)
 * Pattern: Recursive subdivision for adaptive level of detail
 * 
 * What Makes It Ingenious:
 * - Adaptive detail: More detail near camera, less far away
 * - Recursive subdivision: Divide terrain/geometry recursively
 * - Chunk-based systems: Divide world into chunks
 * - Frustum culling: Recursively cull invisible regions
 * - Used in terrain rendering, large worlds, optimization
 * 
 * When to Use:
 * - Large-scale terrain rendering
 * - Open world games
 * - Adaptive mesh refinement
 * - Chunk-based world systems
 * - Performance optimization
 * 
 * Real-World Usage:
 * - Minecraft chunk system
 * - Open world games (GTA, Skyrim)
 * - Terrain rendering engines
 * - Large-scale game worlds
 * - Procedural generation systems
 * 
 * Time Complexity: O(log n) for queries, O(n) for subdivision
 * Space Complexity: O(n) for LOD tree
 */

#include <vector>
#include <memory>
#include <cmath>
#include <iostream>
#include <algorithm>

class RecursiveLODSystem {
public:
    // 3D Point
    struct Point3D {
        float x, y, z;
        Point3D(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
        
        float distance(const Point3D& other) const {
            float dx = x - other.x;
            float dy = y - other.y;
            float dz = z - other.z;
            return std::sqrt(dx * dx + dy * dy + dz * dz);
        }
    };
    
    // Bounding box
    struct AABB {
        Point3D min, max;
        
        AABB(Point3D min_p, Point3D max_p) : min(min_p), max(max_p) {}
        
        Point3D center() const {
            return Point3D((min.x + max.x) / 2.0f,
                          (min.y + max.y) / 2.0f,
                          (min.z + max.z) / 2.0f);
        }
        
        float distance_to_point(const Point3D& p) const {
            Point3D closest(
                std::max(min.x, std::min(p.x, max.x)),
                std::max(min.y, std::min(p.y, max.y)),
                std::max(min.z, std::min(p.z, max.z))
            );
            return p.distance(closest);
        }
    };
    
    // LOD Node
    class LODNode {
    private:
        AABB bounds_;
        int level_;
        int max_level_;
        float lod_threshold_;
        bool is_leaf_;
        
        std::unique_ptr<LODNode> children_[8];  // 8 octants for 3D
        
        void subdivide() {
            if (level_ >= max_level_) {
                return;
            }
            
            Point3D center = bounds_.center();
            Point3D min = bounds_.min;
            Point3D max = bounds_.max;
            
            // Create 8 octants
            children_[0] = std::make_unique<LODNode>(
                AABB(min, center), level_ + 1, max_level_, lod_threshold_);
            children_[1] = std::make_unique<LODNode>(
                AABB(Point3D(center.x, min.y, min.z),
                     Point3D(max.x, center.y, center.z)),
                level_ + 1, max_level_, lod_threshold_);
            children_[2] = std::make_unique<LODNode>(
                AABB(Point3D(min.x, center.y, min.z),
                     Point3D(center.x, max.y, center.z)),
                level_ + 1, max_level_, lod_threshold_);
            children_[3] = std::make_unique<LODNode>(
                AABB(Point3D(center.x, center.y, min.z),
                     Point3D(max.x, max.y, center.z)),
                level_ + 1, max_level_, lod_threshold_);
            children_[4] = std::make_unique<LODNode>(
                AABB(Point3D(min.x, min.y, center.z),
                     Point3D(center.x, center.y, max.z)),
                level_ + 1, max_level_, lod_threshold_);
            children_[5] = std::make_unique<LODNode>(
                AABB(Point3D(center.x, min.y, center.z),
                     Point3D(max.x, center.y, max.z)),
                level_ + 1, max_level_, lod_threshold_);
            children_[6] = std::make_unique<LODNode>(
                AABB(Point3D(min.x, center.y, center.z),
                     Point3D(center.x, max.y, max.z)),
                level_ + 1, max_level_, lod_threshold_);
            children_[7] = std::make_unique<LODNode>(
                AABB(center, max), level_ + 1, max_level_, lod_threshold_);
            
            is_leaf_ = false;
        }
        
    public:
        LODNode(AABB bounds, int level, int max_level, float threshold)
            : bounds_(bounds), level_(level), max_level_(max_level),
              lod_threshold_(threshold), is_leaf_(true) {
            for (int i = 0; i < 8; i++) {
                children_[i] = nullptr;
            }
        }
        
        // Determine LOD level based on distance from camera
        int get_lod_level(const Point3D& camera_pos) const {
            float distance = bounds_.distance_to_point(camera_pos);
            
            // Higher level = more detail (closer to camera)
            // Lower level = less detail (farther from camera)
            if (distance < lod_threshold_ * (1 << level_)) {
                return level_;
            }
            
            return level_ - 1;
        }
        
        // Get nodes to render (recursive)
        void get_render_nodes(const Point3D& camera_pos, 
                             std::vector<LODNode*>& nodes) {
            float distance = bounds_.distance_to_point(camera_pos);
            float threshold = lod_threshold_ * (1 << level_);
            
            if (distance < threshold && !is_leaf_) {
                // Subdivide if needed
                if (children_[0] == nullptr) {
                    const_cast<LODNode*>(this)->subdivide();
                }
                
                // Recursively get children
                for (int i = 0; i < 8; i++) {
                    if (children_[i]) {
                        children_[i]->get_render_nodes(camera_pos, nodes);
                    }
                }
            } else {
                // This node should be rendered at current LOD
                nodes.push_back(const_cast<LODNode*>(this));
            }
        }
        
        AABB get_bounds() const { return bounds_; }
        int get_level() const { return level_; }
        bool is_leaf() const { return is_leaf_; }
    };
    
    // Terrain chunk with LOD
    class TerrainChunk {
    private:
        AABB bounds_;
        int lod_level_;
        std::vector<Point3D> vertices_;
        std::vector<int> indices_;
        
    public:
        TerrainChunk(AABB bounds, int lod) 
            : bounds_(bounds), lod_level_(lod) {
            generate_mesh();
        }
        
        void generate_mesh() {
            // Simplified: generate grid based on LOD level
            int resolution = 2 << lod_level_;  // Higher LOD = more vertices
            
            vertices_.clear();
            indices_.clear();
            
            Point3D min = bounds_.min;
            Point3D max = bounds_.max;
            float step_x = (max.x - min.x) / resolution;
            float step_z = (max.z - min.z) / resolution;
            
            // Generate vertices
            for (int i = 0; i <= resolution; i++) {
                for (int j = 0; j <= resolution; j++) {
                    float x = min.x + i * step_x;
                    float z = min.z + j * step_z;
                    float y = 0.0f;  // Simplified height
                    vertices_.push_back(Point3D(x, y, z));
                }
            }
            
            // Generate indices (triangles)
            for (int i = 0; i < resolution; i++) {
                for (int j = 0; j < resolution; j++) {
                    int top_left = i * (resolution + 1) + j;
                    int top_right = top_left + 1;
                    int bottom_left = (i + 1) * (resolution + 1) + j;
                    int bottom_right = bottom_left + 1;
                    
                    // Two triangles per quad
                    indices_.push_back(top_left);
                    indices_.push_back(bottom_left);
                    indices_.push_back(top_right);
                    
                    indices_.push_back(top_right);
                    indices_.push_back(bottom_left);
                    indices_.push_back(bottom_right);
                }
            }
        }
        
        void set_lod(int lod) {
            if (lod != lod_level_) {
                lod_level_ = lod;
                generate_mesh();
            }
        }
        
        int get_lod() const { return lod_level_; }
        size_t get_vertex_count() const { return vertices_.size(); }
        size_t get_index_count() const { return indices_.size(); }
    };
    
    // LOD Manager
    class LODManager {
    private:
        std::unique_ptr<LODNode> root_;
        Point3D camera_position_;
        float lod_threshold_;
        
    public:
        LODManager(AABB world_bounds, int max_level, float threshold)
            : lod_threshold_(threshold) {
            root_ = std::make_unique<LODNode>(
                world_bounds, 0, max_level, threshold);
        }
        
        void update_camera(const Point3D& pos) {
            camera_position_ = pos;
        }
        
        std::vector<LODNode*> get_visible_nodes() {
            std::vector<LODNode*> nodes;
            if (root_) {
                root_->get_render_nodes(camera_position_, nodes);
            }
            return nodes;
        }
        
        int get_node_count() const {
            // Recursively count nodes
            return count_nodes(root_.get());
        }
        
    private:
        int count_nodes(LODNode* node) const {
            if (!node) return 0;
            int count = 1;
            if (!node->is_leaf()) {
                for (int i = 0; i < 8; i++) {
                    // Would need to access children, simplified here
                }
            }
            return count;
        }
    };
};

// Example usage
int main() {
    using namespace RecursiveLODSystem;
    
    // Create world bounds
    AABB world_bounds(
        Point3D(-100, 0, -100),
        Point3D(100, 0, 100)
    );
    
    // Create LOD manager
    LODManager manager(world_bounds, 4, 10.0f);
    
    // Update camera position
    Point3D camera(0, 10, 0);
    manager.update_camera(camera);
    
    // Get visible nodes
    auto nodes = manager.get_visible_nodes();
    std::cout << "Visible LOD nodes: " << nodes.size() << std::endl;
    
    // Create terrain chunk
    TerrainChunk chunk(world_bounds, 2);
    std::cout << "Terrain chunk vertices: " << chunk.get_vertex_count() << std::endl;
    std::cout << "Terrain chunk indices: " << chunk.get_index_count() << std::endl;
    
    return 0;
}

