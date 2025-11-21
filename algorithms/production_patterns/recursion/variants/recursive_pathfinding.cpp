/*
 * Recursive Pathfinding Algorithms - Game Development
 * 
 * Source: Game pathfinding systems (A*, Dijkstra, IDA*)
 * Pattern: Recursive pathfinding with heuristics
 * 
 * What Makes It Ingenious:
 * - A* algorithm: Optimal pathfinding with heuristics
 * - IDA* (Iterative Deepening A*): Memory-efficient A*
 * - Recursive path reconstruction: Builds path backwards
 * - Heuristic functions: Guide search efficiently
 * - Used in game AI, NPC navigation, route planning
 * 
 * When to Use:
 * - Game AI pathfinding
 * - NPC navigation
 * - Route planning in games
 * - Real-time strategy pathfinding
 * - Grid-based movement systems
 * 
 * Real-World Usage:
 * - Game engines (Unity, Unreal)
 * - RTS game pathfinding
 * - RPG NPC movement
 * - Strategy game unit movement
 * - Tower defense pathfinding
 * 
 * Time Complexity: O(b^d) worst case, O(|V| log |V|) with good heuristic
 * Space Complexity: O(|V|) for visited nodes
 */

#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <limits>

class RecursivePathfinding {
public:
    struct Point {
        int x, y;
        Point(int x = 0, int y = 0) : x(x), y(y) {}
        
        bool operator==(const Point& other) const {
            return x == other.x && y == other.y;
        }
        
        double distance(const Point& other) const {
            int dx = x - other.x;
            int dy = y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }
    };
    
    struct Node {
        Point pos;
        double g_cost;  // Cost from start
        double h_cost;  // Heuristic cost to goal
        double f_cost;  // Total cost (g + h)
        std::shared_ptr<Node> parent;
        
        Node(Point p, double g, double h, std::shared_ptr<Node> p_parent = nullptr)
            : pos(p), g_cost(g), h_cost(h), f_cost(g + h), parent(p_parent) {}
        
        bool operator<(const Node& other) const {
            return f_cost > other.f_cost;  // For priority queue (min-heap)
        }
    };
    
    // A* pathfinding (recursive reconstruction)
    static std::vector<Point> a_star(
        const std::vector<std::vector<int>>& grid,
        Point start,
        Point goal) {
        
        int rows = grid.size();
        int cols = grid[0].size();
        
        // Priority queue for open set
        std::priority_queue<Node> open_set;
        std::unordered_map<int, std::shared_ptr<Node>> open_map;
        std::unordered_set<int> closed_set;
        
        auto hash_point = [cols](const Point& p) {
            return p.y * cols + p.x;
        };
        
        // Initialize start node
        double h_start = heuristic(start, goal);
        auto start_node = std::make_shared<Node>(start, 0.0, h_start);
        open_set.push(*start_node);
        open_map[hash_point(start)] = start_node;
        
        // Possible moves (4-directional)
        std::vector<Point> moves = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
        
        while (!open_set.empty()) {
            Node current = open_set.top();
            open_set.pop();
            
            int current_hash = hash_point(current.pos);
            
            // Remove from open map if exists
            if (open_map.find(current_hash) != open_map.end()) {
                open_map.erase(current_hash);
            }
            
            // Skip if already processed
            if (closed_set.find(current_hash) != closed_set.end()) {
                continue;
            }
            
            closed_set.insert(current_hash);
            
            // Check if goal reached
            if (current.pos == goal) {
                return reconstruct_path(current);
            }
            
            // Explore neighbors
            for (const auto& move : moves) {
                Point neighbor(current.pos.x + move.x, current.pos.y + move.y);
                
                // Check bounds
                if (neighbor.x < 0 || neighbor.x >= cols ||
                    neighbor.y < 0 || neighbor.y >= rows) {
                    continue;
                }
                
                // Check if walkable
                if (grid[neighbor.y][neighbor.x] == 1) {  // 1 = wall
                    continue;
                }
                
                int neighbor_hash = hash_point(neighbor);
                
                // Skip if in closed set
                if (closed_set.find(neighbor_hash) != closed_set.end()) {
                    continue;
                }
                
                // Calculate costs
                double g_new = current.g_cost + 1.0;  // Assuming uniform cost
                double h_new = heuristic(neighbor, goal);
                double f_new = g_new + h_new;
                
                // Check if already in open set
                if (open_map.find(neighbor_hash) != open_map.end()) {
                    auto existing = open_map[neighbor_hash];
                    if (g_new < existing->g_cost) {
                        // Update node
                        existing->g_cost = g_new;
                        existing->f_cost = f_new;
                        existing->parent = std::make_shared<Node>(current);
                        open_set.push(*existing);
                    }
                } else {
                    // Add new node
                    auto neighbor_node = std::make_shared<Node>(
                        neighbor, g_new, h_new,
                        std::make_shared<Node>(current));
                    open_set.push(*neighbor_node);
                    open_map[neighbor_hash] = neighbor_node;
                }
            }
        }
        
        // No path found
        return {};
    }
    
    // Recursive path reconstruction
    static std::vector<Point> reconstruct_path(const Node& node) {
        std::vector<Point> path;
        reconstruct_path_recursive(node, path);
        std::reverse(path.begin(), path.end());
        return path;
    }
    
private:
    static void reconstruct_path_recursive(const Node& node, std::vector<Point>& path) {
        path.push_back(node.pos);
        if (node.parent) {
            reconstruct_path_recursive(*node.parent, path);
        }
    }
    
    // Heuristic function (Euclidean distance)
    static double heuristic(const Point& a, const Point& b) {
        return a.distance(b);
    }
    
public:
    // IDA* (Iterative Deepening A*) - memory efficient
    static std::vector<Point> ida_star(
        const std::vector<std::vector<int>>& grid,
        Point start,
        Point goal) {
        
        double threshold = heuristic(start, goal);
        
        while (true) {
            std::vector<Point> path;
            double result = ida_star_search(grid, start, goal, 0.0, threshold, path);
            
            if (result == -1.0) {
                // Path found
                return path;
            }
            
            if (result == std::numeric_limits<double>::infinity()) {
                // No path exists
                return {};
            }
            
            // Update threshold for next iteration
            threshold = result;
        }
    }
    
private:
    static double ida_star_search(
        const std::vector<std::vector<int>>& grid,
        Point current,
        Point goal,
        double g_cost,
        double threshold,
        std::vector<Point>& path) {
        
        double f_cost = g_cost + heuristic(current, goal);
        
        if (f_cost > threshold) {
            return f_cost;
        }
        
        if (current == goal) {
            path.push_back(current);
            return -1.0;  // Found
        }
        
        double min_cost = std::numeric_limits<double>::infinity();
        path.push_back(current);
        
        std::vector<Point> moves = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
        int rows = grid.size();
        int cols = grid[0].size();
        
        for (const auto& move : moves) {
            Point neighbor(current.x + move.x, current.y + move.y);
            
            if (neighbor.x < 0 || neighbor.x >= cols ||
                neighbor.y < 0 || neighbor.y >= rows) {
                continue;
            }
            
            if (grid[neighbor.y][neighbor.x] == 1) {
                continue;
            }
            
            // Check if already in path (avoid cycles)
            bool in_path = false;
            for (const auto& p : path) {
                if (p == neighbor) {
                    in_path = true;
                    break;
                }
            }
            if (in_path) continue;
            
            double result = ida_star_search(grid, neighbor, goal, 
                                           g_cost + 1.0, threshold, path);
            
            if (result == -1.0) {
                return -1.0;  // Found
            }
            
            if (result < min_cost) {
                min_cost = result;
            }
        }
        
        path.pop_back();  // Backtrack
        return min_cost;
    }
    
public:
    // Recursive Dijkstra (similar to A* but without heuristic)
    static std::vector<Point> dijkstra(
        const std::vector<std::vector<int>>& grid,
        Point start,
        Point goal) {
        
        int rows = grid.size();
        int cols = grid[0].size();
        
        std::unordered_map<int, double> dist;
        std::unordered_map<int, std::shared_ptr<Node>> prev;
        std::priority_queue<std::pair<double, Point>> pq;
        
        auto hash_point = [cols](const Point& p) {
            return p.y * cols + p.x;
        };
        
        // Initialize distances
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (grid[i][j] != 1) {
                    dist[hash_point(Point(j, i))] = std::numeric_limits<double>::infinity();
                }
            }
        }
        
        dist[hash_point(start)] = 0.0;
        pq.push({0.0, start});
        
        std::vector<Point> moves = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
        
        while (!pq.empty()) {
            auto [d, current] = pq.top();
            pq.pop();
            d = -d;  // Negate because priority queue is max-heap
            
            if (current == goal) {
                return reconstruct_path_dijkstra(prev, start, goal, cols);
            }
            
            int current_hash = hash_point(current);
            if (d > dist[current_hash]) {
                continue;
            }
            
            for (const auto& move : moves) {
                Point neighbor(current.x + move.x, current.y + move.y);
                
                if (neighbor.x < 0 || neighbor.x >= cols ||
                    neighbor.y < 0 || neighbor.y >= rows) {
                    continue;
                }
                
                if (grid[neighbor.y][neighbor.x] == 1) {
                    continue;
                }
                
                int neighbor_hash = hash_point(neighbor);
                double alt = dist[current_hash] + 1.0;
                
                if (alt < dist[neighbor_hash]) {
                    dist[neighbor_hash] = alt;
                    prev[neighbor_hash] = std::make_shared<Node>(current, alt, 0.0);
                    pq.push({-alt, neighbor});
                }
            }
        }
        
        return {};
    }
    
private:
    static std::vector<Point> reconstruct_path_dijkstra(
        const std::unordered_map<int, std::shared_ptr<Node>>& prev,
        Point start,
        Point goal,
        int cols) {
        
        std::vector<Point> path;
        auto hash_point = [cols](const Point& p) {
            return p.y * cols + p.x;
        };
        
        Point current = goal;
        while (current != start) {
            path.push_back(current);
            int hash = hash_point(current);
            if (prev.find(hash) != prev.end() && prev.at(hash)) {
                current = prev.at(hash)->pos;
            } else {
                return {};  // No path
            }
        }
        path.push_back(start);
        std::reverse(path.begin(), path.end());
        return path;
    }
};

// Example usage
int main() {
    // Create a simple grid (0 = walkable, 1 = wall)
    std::vector<std::vector<int>> grid = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0, 1, 0},
        {0, 0, 0, 0, 0, 1, 0},
        {0, 1, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0}
    };
    
    RecursivePathfinding::Point start(0, 0);
    RecursivePathfinding::Point goal(6, 4);
    
    // A* pathfinding
    auto path = RecursivePathfinding::a_star(grid, start, goal);
    
    std::cout << "A* Path found with " << path.size() << " steps:" << std::endl;
    for (const auto& p : path) {
        std::cout << "(" << p.x << ", " << p.y << ") ";
    }
    std::cout << std::endl;
    
    // IDA* pathfinding
    auto path2 = RecursivePathfinding::ida_star(grid, start, goal);
    std::cout << "\nIDA* Path found with " << path2.size() << " steps" << std::endl;
    
    return 0;
}

