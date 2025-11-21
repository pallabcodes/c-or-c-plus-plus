/*
 * Recursive Procedural Generation - Game Development
 * 
 * Source: Game development procedural generation techniques
 * Pattern: Recursive algorithms for generating game content
 * 
 * What Makes It Ingenious:
 * - Recursive subdivision: Divide space recursively
 * - Binary Space Partitioning (BSP): Recursive space division
 * - Recursive dungeon generation: Create rooms and corridors
 * - Fractal terrain generation: Recursive height maps
 * - Used in roguelikes, procedural games, level generation
 * 
 * When to Use:
 * - Procedural dungeon generation
 * - Terrain generation
 * - Level generation
 * - Random map creation
 * - Roguelike game development
 * 
 * Real-World Usage:
 * - Roguelike games (Dwarf Fortress, Nethack)
 * - Procedural games (Minecraft, No Man's Sky)
 * - Level generators
 * - Terrain systems
 * - Random map generation
 * 
 * Time Complexity: O(n log n) for BSP, O(n) for simple generation
 * Space Complexity: O(n) for recursion depth
 */

#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <memory>

class RecursiveProceduralGeneration {
public:
    // Binary Space Partitioning (BSP) for dungeon generation
    struct BSPNode {
        int x, y, width, height;
        std::unique_ptr<BSPNode> left;
        std::unique_ptr<BSPNode> right;
        bool is_leaf;
        
        BSPNode(int x, int y, int w, int h)
            : x(x), y(y), width(w), height(h), 
              left(nullptr), right(nullptr), is_leaf(true) {}
    };
    
    class BSPDungeonGenerator {
    private:
        std::mt19937 rng;
        int min_room_size;
        int max_room_size;
        
        // Recursively split space
        void split_node(std::unique_ptr<BSPNode>& node, int depth, int max_depth) {
            if (depth >= max_depth || node->width < min_room_size * 2 ||
                node->height < min_room_size * 2) {
                return;  // Stop splitting
            }
            
            bool horizontal = (node->width < node->height) ||
                             (node->width == node->height && rng() % 2 == 0);
            
            if (horizontal) {
                // Split horizontally
                int split = min_room_size + 
                           (rng() % (node->height - 2 * min_room_size));
                
                node->left = std::make_unique<BSPNode>(
                    node->x, node->y, node->width, split);
                node->right = std::make_unique<BSPNode>(
                    node->x, node->y + split, node->width, node->height - split);
            } else {
                // Split vertically
                int split = min_room_size + 
                           (rng() % (node->width - 2 * min_room_size));
                
                node->left = std::make_unique<BSPNode>(
                    node->x, node->y, split, node->height);
                node->right = std::make_unique<BSPNode>(
                    node->x + split, node->y, node->width - split, node->height);
            }
            
            node->is_leaf = false;
            
            // Recursively split children
            split_node(node->left, depth + 1, max_depth);
            split_node(node->right, depth + 1, max_depth);
        }
        
        // Create rooms in leaf nodes
        void create_rooms(std::unique_ptr<BSPNode>& node, 
                         std::vector<std::pair<int, int>>& rooms) {
            if (!node) return;
            
            if (node->is_leaf) {
                // Create room in leaf
                int room_width = min_room_size + (rng() % (node->width - min_room_size));
                int room_height = min_room_size + (rng() % (node->height - min_room_size));
                int room_x = node->x + (rng() % (node->width - room_width));
                int room_y = node->y + (rng() % (node->height - room_height));
                
                rooms.push_back({room_x, room_y});
                rooms.push_back({room_width, room_height});
            } else {
                create_rooms(node->left, rooms);
                create_rooms(node->right, rooms);
            }
        }
        
    public:
        BSPDungeonGenerator(int seed, int min_size, int max_size)
            : rng(seed), min_room_size(min_size), max_room_size(max_size) {}
        
        std::unique_ptr<BSPNode> generate_dungeon(int width, int height, int max_depth) {
            auto root = std::make_unique<BSPNode>(0, 0, width, height);
            split_node(root, 0, max_depth);
            return root;
        }
        
        std::vector<std::pair<int, int>> get_rooms(std::unique_ptr<BSPNode>& root) {
            std::vector<std::pair<int, int>> rooms;
            create_rooms(root, rooms);
            return rooms;
        }
    };
    
    // Recursive maze generation (backtracking)
    class RecursiveMazeGenerator {
    private:
        std::mt19937 rng;
        std::vector<std::vector<bool>> maze;
        int rows, cols;
        
        void carve_passage(int row, int col) {
            maze[row][col] = true;  // Mark as path
            
            // Shuffle directions
            std::vector<std::pair<int, int>> directions = {
                {0, 1}, {1, 0}, {0, -1}, {-1, 0}
            };
            std::shuffle(directions.begin(), directions.end(), rng);
            
            for (const auto& dir : directions) {
                int new_row = row + dir.first * 2;
                int new_col = col + dir.second * 2;
                
                if (new_row > 0 && new_row < rows - 1 &&
                    new_col > 0 && new_col < cols - 1 &&
                    !maze[new_row][new_col]) {
                    
                    // Carve wall between cells
                    maze[row + dir.first][col + dir.second] = true;
                    
                    // Recursively carve
                    carve_passage(new_row, new_col);
                }
            }
        }
        
    public:
        RecursiveMazeGenerator(int seed, int r, int c)
            : rng(seed), rows(r), cols(c) {
            maze.resize(rows, std::vector<bool>(cols, false));
        }
        
        std::vector<std::vector<bool>> generate() {
            // Start from (1, 1) - must be odd coordinates
            if (rows > 1 && cols > 1) {
                carve_passage(1, 1);
            }
            return maze;
        }
    };
    
    // Fractal terrain generation (midpoint displacement)
    class FractalTerrainGenerator {
    private:
        std::mt19937 rng;
        std::vector<std::vector<double>> height_map;
        double roughness;
        
        void midpoint_displacement(int x1, int y1, int x2, int y2, double range) {
            if (x2 - x1 < 2 && y2 - y1 < 2) {
                return;
            }
            
            int mid_x = (x1 + x2) / 2;
            int mid_y = (y1 + y2) / 2;
            
            // Calculate midpoint height
            double avg = (height_map[y1][x1] + height_map[y1][x2] +
                         height_map[y2][x1] + height_map[y2][x2]) / 4.0;
            
            std::uniform_real_distribution<double> dist(-range, range);
            height_map[mid_y][mid_x] = avg + dist(rng);
            
            // Calculate edge midpoints
            if (x2 - x1 > 1) {
                height_map[y1][mid_x] = (height_map[y1][x1] + height_map[y1][x2]) / 2.0 +
                                       dist(rng) * roughness;
                height_map[y2][mid_x] = (height_map[y2][x1] + height_map[y2][x2]) / 2.0 +
                                       dist(rng) * roughness;
            }
            
            if (y2 - y1 > 1) {
                height_map[mid_y][x1] = (height_map[y1][x1] + height_map[y2][x1]) / 2.0 +
                                       dist(rng) * roughness;
                height_map[mid_y][x2] = (height_map[y1][x2] + height_map[y2][x2]) / 2.0 +
                                       dist(rng) * roughness;
            }
            
            // Recursively subdivide
            double new_range = range * roughness;
            midpoint_displacement(x1, y1, mid_x, mid_y, new_range);
            midpoint_displacement(mid_x, y1, x2, mid_y, new_range);
            midpoint_displacement(x1, mid_y, mid_x, y2, new_range);
            midpoint_displacement(mid_x, mid_y, x2, y2, new_range);
        }
        
    public:
        FractalTerrainGenerator(int seed, int size, double rough)
            : rng(seed), roughness(rough) {
            height_map.resize(size, std::vector<double>(size, 0.0));
        }
        
        std::vector<std::vector<double>> generate() {
            int size = height_map.size();
            
            // Initialize corners
            std::uniform_real_distribution<double> init_dist(0.0, 1.0);
            height_map[0][0] = init_dist(rng);
            height_map[0][size - 1] = init_dist(rng);
            height_map[size - 1][0] = init_dist(rng);
            height_map[size - 1][size - 1] = init_dist(rng);
            
            // Recursively generate
            midpoint_displacement(0, 0, size - 1, size - 1, 1.0);
            
            return height_map;
        }
    };
    
    // Recursive room placement
    class RecursiveRoomPlacer {
    private:
        std::mt19937 rng;
        std::vector<std::vector<bool>> map;
        int rows, cols;
        
        bool can_place_room(int x, int y, int w, int h) {
            if (x + w >= cols || y + h >= rows) return false;
            
            for (int i = y; i < y + h; i++) {
                for (int j = x; j < x + w; j++) {
                    if (map[i][j]) return false;  // Overlap
                }
            }
            return true;
        }
        
        void place_room(int x, int y, int w, int h) {
            for (int i = y; i < y + h; i++) {
                for (int j = x; j < x + w; j++) {
                    map[i][j] = true;
                }
            }
        }
        
        bool recursive_place(int attempts, int min_size, int max_size) {
            if (attempts <= 0) return false;
            
            int w = min_size + (rng() % (max_size - min_size));
            int h = min_size + (rng() % (max_size - min_size));
            int x = rng() % (cols - w);
            int y = rng() % (rows - h);
            
            if (can_place_room(x, y, w, h)) {
                place_room(x, y, w, h);
                return true;
            }
            
            return recursive_place(attempts - 1, min_size, max_size);
        }
        
    public:
        RecursiveRoomPlacer(int seed, int r, int c)
            : rng(seed), rows(r), cols(c) {
            map.resize(rows, std::vector<bool>(cols, false));
        }
        
        void generate_rooms(int num_rooms, int min_size, int max_size) {
            for (int i = 0; i < num_rooms; i++) {
                recursive_place(100, min_size, max_size);
            }
        }
        
        std::vector<std::vector<bool>> get_map() const {
            return map;
        }
    };
};

// Example usage
int main() {
    // BSP Dungeon Generation
    std::cout << "BSP Dungeon Generation:" << std::endl;
    RecursiveProceduralGeneration::BSPDungeonGenerator bsp_gen(12345, 4, 8);
    auto dungeon = bsp_gen.generate_dungeon(64, 64, 5);
    auto rooms = bsp_gen.get_rooms(dungeon);
    std::cout << "Generated " << rooms.size() / 2 << " rooms" << std::endl;
    
    // Maze Generation
    std::cout << "\nRecursive Maze Generation:" << std::endl;
    RecursiveProceduralGeneration::RecursiveMazeGenerator maze_gen(54321, 21, 21);
    auto maze = maze_gen.generate();
    std::cout << "Generated " << maze.size() << "x" << maze[0].size() << " maze" << std::endl;
    
    // Fractal Terrain
    std::cout << "\nFractal Terrain Generation:" << std::endl;
    RecursiveProceduralGeneration::FractalTerrainGenerator terrain_gen(11111, 65, 0.5);
    auto terrain = terrain_gen.generate();
    std::cout << "Generated " << terrain.size() << "x" << terrain[0].size() << " terrain" << std::endl;
    
    // Room Placement
    std::cout << "\nRecursive Room Placement:" << std::endl;
    RecursiveProceduralGeneration::RecursiveRoomPlacer room_gen(22222, 50, 50);
    room_gen.generate_rooms(10, 3, 8);
    auto room_map = room_gen.get_map();
    std::cout << "Generated room map" << std::endl;
    
    return 0;
}

