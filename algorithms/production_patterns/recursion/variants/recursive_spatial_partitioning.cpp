/*
 * Recursive Spatial Partitioning (Quadtree/Octree) - Game Development
 * 
 * Source: Game engines (Unity, Unreal, custom engines)
 * Pattern: Recursive space subdivision for collision detection and culling
 * 
 * What Makes It Ingenious:
 * - Quadtree: 2D space subdivision into 4 quadrants
 * - Octree: 3D space subdivision into 8 octants
 * - Recursive subdivision: Divide until threshold reached
 * - Efficient collision detection: Only check nearby objects
 * - Frustum culling: Quickly eliminate objects outside view
 * - Used in physics engines, rendering, spatial queries
 * 
 * When to Use:
 * - Collision detection in games
 * - Frustum culling for rendering
 * - Spatial queries (find objects in region)
 * - Physics optimization
 * - Large-scale game worlds
 * 
 * Real-World Usage:
 * - Game engines (Unity, Unreal Engine)
 * - Physics engines (Bullet, Box2D)
 * - Rendering systems
 * - Open world games
 * - RTS games for unit management
 * 
 * Time Complexity: O(log n) average for queries, O(n log n) for construction
 * Space Complexity: O(n) for tree nodes
 */

#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include <cmath>

class RecursiveSpatialPartitioning {
public:
    // 2D Point
    struct Point2D {
        float x, y;
        Point2D(float x = 0, float y = 0) : x(x), y(y) {}
    };
    
    // 2D Bounding Box
    struct AABB2D {
        Point2D min, max;
        
        AABB2D(Point2D min_p, Point2D max_p) : min(min_p), max(max_p) {}
        
        bool contains(const Point2D& p) const {
            return p.x >= min.x && p.x <= max.x &&
                   p.y >= min.y && p.y <= max.y;
        }
        
        bool intersects(const AABB2D& other) const {
            return !(max.x < other.min.x || min.x > other.max.x ||
                    max.y < other.min.y || min.y > other.max.y);
        }
        
        Point2D center() const {
            return Point2D((min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f);
        }
    };
    
    // Game object with position and bounds
    struct GameObject {
        int id;
        Point2D position;
        AABB2D bounds;
        
        GameObject(int i, Point2D pos, AABB2D b)
            : id(i), position(pos), bounds(b) {}
    };
    
    // Quadtree Node
    class Quadtree {
    private:
        AABB2D boundary_;
        int capacity_;
        std::vector<GameObject> objects_;
        bool divided_;
        
        std::unique_ptr<Quadtree> northwest_;
        std::unique_ptr<Quadtree> northeast_;
        std::unique_ptr<Quadtree> southwest_;
        std::unique_ptr<Quadtree> southeast_;
        
        // Recursively subdivide
        void subdivide() {
            Point2D center = boundary_.center();
            float half_width = (boundary_.max.x - boundary_.min.x) / 2.0f;
            float half_height = (boundary_.max.y - boundary_.min.y) / 2.0f;
            
            AABB2D nw(boundary_.min, Point2D(center.x, center.y));
            AABB2D ne(Point2D(center.x, boundary_.min.y),
                     Point2D(boundary_.max.x, center.y));
            AABB2D sw(Point2D(boundary_.min.x, center.y),
                     Point2D(center.x, boundary_.max.y));
            AABB2D se(Point2D(center.x, center.y), boundary_.max);
            
            northwest_ = std::make_unique<Quadtree>(nw, capacity_);
            northeast_ = std::make_unique<Quadtree>(ne, capacity_);
            southwest_ = std::make_unique<Quadtree>(sw, capacity_);
            southeast_ = std::make_unique<Quadtree>(se, capacity_);
            
            divided_ = true;
        }
        
    public:
        Quadtree(AABB2D boundary, int capacity = 4)
            : boundary_(boundary), capacity_(capacity), divided_(false) {}
        
        // Insert object recursively
        bool insert(const GameObject& obj) {
            // Check if object is in this boundary
            if (!boundary_.contains(obj.position)) {
                return false;
            }
            
            // If not at capacity, add to this node
            if (objects_.size() < capacity_) {
                objects_.push_back(obj);
                return true;
            }
            
            // Subdivide if not already divided
            if (!divided_) {
                subdivide();
            }
            
            // Try to insert into children
            if (northwest_->insert(obj)) return true;
            if (northeast_->insert(obj)) return true;
            if (southwest_->insert(obj)) return true;
            if (southeast_->insert(obj)) return true;
            
            // Should not happen if boundary check passed
            return false;
        }
        
        // Query objects in range (recursive)
        void query(const AABB2D& range, std::vector<GameObject>& found) {
            // Check if range intersects this boundary
            if (!boundary_.intersects(range)) {
                return;
            }
            
            // Check objects in this node
            for (const auto& obj : objects_) {
                if (range.contains(obj.position)) {
                    found.push_back(obj);
                }
            }
            
            // Query children if divided
            if (divided_) {
                northwest_->query(range, found);
                northeast_->query(range, found);
                southwest_->query(range, found);
                southeast_->query(range, found);
            }
        }
        
        // Find nearest neighbor (recursive)
        GameObject* nearest_neighbor(const Point2D& point, 
                                    GameObject* best = nullptr,
                                    float best_dist = std::numeric_limits<float>::max()) {
            // Check if point is in this boundary
            if (!boundary_.contains(point)) {
                return best;
            }
            
            // Check objects in this node
            for (auto& obj : objects_) {
                float dx = obj.position.x - point.x;
                float dy = obj.position.y - point.y;
                float dist = std::sqrt(dx * dx + dy * dy);
                
                if (dist < best_dist) {
                    best_dist = dist;
                    best = &obj;
                }
            }
            
            // Check children if divided
            if (divided_) {
                best = northwest_->nearest_neighbor(point, best, best_dist);
                best = northeast_->nearest_neighbor(point, best, best_dist);
                best = southwest_->nearest_neighbor(point, best, best_dist);
                best = southeast_->nearest_neighbor(point, best, best_dist);
            }
            
            return best;
        }
        
        // Clear tree recursively
        void clear() {
            objects_.clear();
            if (divided_) {
                northwest_->clear();
                northeast_->clear();
                southwest_->clear();
                southeast_->clear();
                divided_ = false;
            }
        }
        
        // Get all objects recursively
        void get_all_objects(std::vector<GameObject>& all) {
            all.insert(all.end(), objects_.begin(), objects_.end());
            if (divided_) {
                northwest_->get_all_objects(all);
                northeast_->get_all_objects(all);
                southwest_->get_all_objects(all);
                southeast_->get_all_objects(all);
            }
        }
    };
    
    // 3D Point
    struct Point3D {
        float x, y, z;
        Point3D(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
    };
    
    // 3D Bounding Box
    struct AABB3D {
        Point3D min, max;
        
        AABB3D(Point3D min_p, Point3D max_p) : min(min_p), max(max_p) {}
        
        bool contains(const Point3D& p) const {
            return p.x >= min.x && p.x <= max.x &&
                   p.y >= min.y && p.y <= max.y &&
                   p.z >= min.z && p.z <= max.z;
        }
        
        bool intersects(const AABB3D& other) const {
            return !(max.x < other.min.x || min.x > other.max.x ||
                    max.y < other.min.y || min.y > other.max.y ||
                    max.z < other.min.z || min.z > other.max.z);
        }
        
        Point3D center() const {
            return Point3D((min.x + max.x) / 2.0f,
                          (min.y + max.y) / 2.0f,
                          (min.z + max.z) / 2.0f);
        }
    };
    
    // Octree Node (3D)
    class Octree {
    private:
        AABB3D boundary_;
        int capacity_;
        std::vector<GameObject> objects_;
        bool divided_;
        
        std::unique_ptr<Octree> children_[8];  // 8 octants
        
        void subdivide() {
            Point3D center = boundary_.center();
            float half_x = (boundary_.max.x - boundary_.min.x) / 2.0f;
            float half_y = (boundary_.max.y - boundary_.min.y) / 2.0f;
            float half_z = (boundary_.max.z - boundary_.min.z) / 2.0f;
            
            // Create 8 octants
            children_[0] = std::make_unique<Octree>(
                AABB3D(boundary_.min, center), capacity_);
            children_[1] = std::make_unique<Octree>(
                AABB3D(Point3D(center.x, boundary_.min.y, boundary_.min.z),
                       Point3D(boundary_.max.x, center.y, center.z)), capacity_);
            children_[2] = std::make_unique<Octree>(
                AABB3D(Point3D(boundary_.min.x, center.y, boundary_.min.z),
                       Point3D(center.x, boundary_.max.y, center.z)), capacity_);
            children_[3] = std::make_unique<Octree>(
                AABB3D(Point3D(center.x, center.y, boundary_.min.z),
                       Point3D(boundary_.max.x, boundary_.max.y, center.z)), capacity_);
            children_[4] = std::make_unique<Octree>(
                AABB3D(Point3D(boundary_.min.x, boundary_.min.y, center.z),
                       Point3D(center.x, center.y, boundary_.max.z)), capacity_);
            children_[5] = std::make_unique<Octree>(
                AABB3D(Point3D(center.x, boundary_.min.y, center.z),
                       Point3D(boundary_.max.x, center.y, boundary_.max.z)), capacity_);
            children_[6] = std::make_unique<Octree>(
                AABB3D(Point3D(boundary_.min.x, center.y, center.z),
                       Point3D(center.x, boundary_.max.y, boundary_.max.z)), capacity_);
            children_[7] = std::make_unique<Octree>(
                AABB3D(center, boundary_.max), capacity_);
            
            divided_ = true;
        }
        
    public:
        Octree(AABB3D boundary, int capacity = 4)
            : boundary_(boundary), capacity_(capacity), divided_(false) {
            for (int i = 0; i < 8; i++) {
                children_[i] = nullptr;
            }
        }
        
        bool insert(const GameObject& obj) {
            if (!boundary_.contains(obj.position)) {
                return false;
            }
            
            if (objects_.size() < capacity_) {
                objects_.push_back(obj);
                return true;
            }
            
            if (!divided_) {
                subdivide();
            }
            
            for (int i = 0; i < 8; i++) {
                if (children_[i]->insert(obj)) {
                    return true;
                }
            }
            
            return false;
        }
        
        void query(const AABB3D& range, std::vector<GameObject>& found) {
            if (!boundary_.intersects(range)) {
                return;
            }
            
            for (const auto& obj : objects_) {
                if (range.contains(obj.position)) {
                    found.push_back(obj);
                }
            }
            
            if (divided_) {
                for (int i = 0; i < 8; i++) {
                    children_[i]->query(range, found);
                }
            }
        }
    };
};

// Example usage
int main() {
    // Create quadtree
    RecursiveSpatialPartitioning::AABB2D boundary(
        RecursiveSpatialPartitioning::Point2D(0, 0),
        RecursiveSpatialPartitioning::Point2D(100, 100));
    
    RecursiveSpatialPartitioning::Quadtree quadtree(boundary, 4);
    
    // Insert some objects
    for (int i = 0; i < 20; i++) {
        float x = static_cast<float>(rand() % 100);
        float y = static_cast<float>(rand() % 100);
        RecursiveSpatialPartitioning::Point2D pos(x, y);
        RecursiveSpatialPartitioning::AABB2D bounds(pos, pos);
        RecursiveSpatialPartitioning::GameObject obj(i, pos, bounds);
        quadtree.insert(obj);
    }
    
    // Query objects in range
    RecursiveSpatialPartitioning::AABB2D query_range(
        RecursiveSpatialPartitioning::Point2D(20, 20),
        RecursiveSpatialPartitioning::Point2D(40, 40));
    
    std::vector<RecursiveSpatialPartitioning::GameObject> found;
    quadtree.query(query_range, found);
    
    std::cout << "Found " << found.size() << " objects in query range" << std::endl;
    
    // Find nearest neighbor
    RecursiveSpatialPartitioning::Point2D search_point(50, 50);
    auto nearest = quadtree.nearest_neighbor(search_point);
    if (nearest) {
        std::cout << "Nearest neighbor ID: " << nearest->id << std::endl;
    }
    
    return 0;
}
