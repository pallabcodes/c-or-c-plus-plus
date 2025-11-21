/*
 * Hashlife Algorithm - Game Development
 * 
 * Source: Bill Gosper's Hashlife algorithm for Conway's Game of Life
 * Pattern: Memoized recursive algorithm for cellular automata
 * 
 * What Makes It Ingenious:
 * - Memoization: Stores previously computed states
 * - Recursive quad-tree structure: Hierarchical representation
 * - Time skipping: Can compute future states efficiently
 * - Used in Conway's Game of Life and other cellular automata
 * - Dramatically faster than naive simulation
 * 
 * When to Use:
 * - Conway's Game of Life simulation
 * - Cellular automata
 * - Pattern evolution over many generations
 * - Long-term simulation of grid-based systems
 * 
 * Real-World Usage:
 * - Game of Life simulators
 * - Cellular automata research
 * - Pattern analysis tools
 * 
 * Time Complexity: O(log n) per generation for stable patterns
 * Space Complexity: O(n) for memoization
 */

#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>
#include <functional>

class Hashlife {
public:
    // Quad-tree node for 2^n x 2^n grid
    struct QuadNode {
        int level;  // 2^level x 2^level grid
        bool alive;
        std::shared_ptr<QuadNode> nw, ne, sw, se;  // Quadrants
        
        QuadNode(int lvl, bool alv = false)
            : level(lvl), alive(alv), nw(nullptr), ne(nullptr), 
              sw(nullptr), se(nullptr) {}
        
        bool is_leaf() const {
            return level == 0;
        }
        
        // Create leaf node
        static std::shared_ptr<QuadNode> create_leaf(bool alive) {
            return std::make_shared<QuadNode>(0, alive);
        }
        
        // Create node from four quadrants
        static std::shared_ptr<QuadNode> create_node(
            std::shared_ptr<QuadNode> nw,
            std::shared_ptr<QuadNode> ne,
            std::shared_ptr<QuadNode> sw,
            std::shared_ptr<QuadNode> se) {
            
            auto node = std::make_shared<QuadNode>(nw->level + 1);
            node->nw = nw;
            node->ne = ne;
            node->sw = sw;
            node->se = se;
            return node;
        }
    };
    
    class HashlifeSimulator {
    private:
        // Memoization table
        std::unordered_map<std::string, std::shared_ptr<QuadNode>> memo_;
        
        // Hash function for node
        std::string hash_node(std::shared_ptr<QuadNode> node) {
            if (!node) return "null";
            if (node->is_leaf()) {
                return node->alive ? "1" : "0";
            }
            return hash_node(node->nw) + "," +
                   hash_node(node->ne) + "," +
                   hash_node(node->sw) + "," +
                   hash_node(node->se);
        }
        
        // Get or create memoized node
        std::shared_ptr<QuadNode> get_memoized(
            std::shared_ptr<QuadNode> nw,
            std::shared_ptr<QuadNode> ne,
            std::shared_ptr<QuadNode> sw,
            std::shared_ptr<QuadNode> se) {
            
            std::string key = hash_node(nw) + "|" + hash_node(ne) + "|" +
                             hash_node(sw) + "|" + hash_node(se);
            
            if (memo_.find(key) != memo_.end()) {
                return memo_[key];
            }
            
            auto node = QuadNode::create_node(nw, ne, sw, se);
            memo_[key] = node;
            return node;
        }
        
        // Get center sub-quadrant
        std::shared_ptr<QuadNode> get_center(std::shared_ptr<QuadNode> node) {
            if (node->is_leaf()) {
                return node;
            }
            
            return get_memoized(
                node->nw->se,
                node->ne->sw,
                node->sw->ne,
                node->se->nw
            );
        }
        
        // Get center horizontal
        std::shared_ptr<QuadNode> get_center_horizontal(
            std::shared_ptr<QuadNode> w, std::shared_ptr<QuadNode> e) {
            if (w->is_leaf() && e->is_leaf()) {
                return get_memoized(w, e, w, e);
            }
            
            return get_memoized(
                w->ne->se,
                e->nw->sw,
                w->se->ne,
                e->sw->nw
            );
        }
        
        // Get center vertical
        std::shared_ptr<QuadNode> get_center_vertical(
            std::shared_ptr<QuadNode> n, std::shared_ptr<QuadNode> s) {
            if (n->is_leaf() && s->is_leaf()) {
                return get_memoized(n, n, s, s);
            }
            
            return get_memoized(
                n->sw->se,
                n->se->sw,
                s->nw->ne,
                s->ne->nw
            );
        }
        
        // Get center center
        std::shared_ptr<QuadNode> get_center_center(
            std::shared_ptr<QuadNode> nw,
            std::shared_ptr<QuadNode> ne,
            std::shared_ptr<QuadNode> sw,
            std::shared_ptr<QuadNode> se) {
            
            return get_memoized(
                nw->se->se,
                ne->sw->sw,
                sw->ne->ne,
                se->nw->nw
            );
        }
        
        // Game of Life rule: Count neighbors
        int count_neighbors(std::shared_ptr<QuadNode> node, int row, int col) {
            if (node->is_leaf()) {
                return node->alive ? 1 : 0;
            }
            
            int half = 1 << (node->level - 1);
            int count = 0;
            
            if (row < half && col < half) {
                count += count_neighbors(node->nw, row, col);
            } else if (row < half && col >= half) {
                count += count_neighbors(node->ne, row, col - half);
            } else if (row >= half && col < half) {
                count += count_neighbors(node->sw, row - half, col);
            } else {
                count += count_neighbors(node->se, row - half, col - half);
            }
            
            return count;
        }
        
        // Next generation for leaf
        std::shared_ptr<QuadNode> next_generation_leaf(
            std::shared_ptr<QuadNode> nw,
            std::shared_ptr<QuadNode> ne,
            std::shared_ptr<QuadNode> sw,
            std::shared_ptr<QuadNode> se) {
            
            // Count neighbors for center cell
            int neighbors = 0;
            if (nw && nw->se) neighbors += nw->se->alive ? 1 : 0;
            if (ne && ne->sw) neighbors += ne->sw->alive ? 1 : 0;
            if (sw && sw->ne) neighbors += sw->ne->alive ? 1 : 0;
            if (se && se->nw) neighbors += se->nw->alive ? 1 : 0;
            
            // Count from other quadrants
            if (nw && nw->se) neighbors += nw->se->alive ? 1 : 0;
            if (ne && ne->sw) neighbors += ne->sw->alive ? 1 : 0;
            if (sw && sw->ne) neighbors += sw->ne->alive ? 1 : 0;
            if (se && se->nw) neighbors += se->nw->alive ? 1 : 0;
            
            // Game of Life rules
            bool center_alive = (se && se->nw) ? se->nw->alive : false;
            bool next_alive = false;
            
            if (center_alive) {
                next_alive = (neighbors == 2 || neighbors == 3);
            } else {
                next_alive = (neighbors == 3);
            }
            
            return QuadNode::create_leaf(next_alive);
        }
        
        // Next generation recursively
        std::shared_ptr<QuadNode> next_generation(std::shared_ptr<QuadNode> node) {
            if (node->is_leaf()) {
                return node;
            }
            
            // Recursively compute next generation for sub-quadrants
            auto nw_next = next_generation(get_center(node->nw));
            auto ne_next = next_generation(get_center(node->ne));
            auto sw_next = next_generation(get_center(node->sw));
            auto se_next = next_generation(get_center(node->se));
            
            // Combine results
            return get_memoized(nw_next, ne_next, sw_next, se_next);
        }
        
    public:
        // Evolve pattern by n generations
        std::shared_ptr<QuadNode> evolve(
            std::shared_ptr<QuadNode> node, int generations) {
            
            if (generations == 0) {
                return node;
            }
            
            if (generations == 1) {
                return next_generation(node);
            }
            
            // Time skipping: evolve by 2^(level-2) generations
            int skip = 1 << (node->level - 2);
            if (skip > generations) {
                skip = generations;
            }
            
            // Recursively evolve
            auto evolved = evolve(node, skip);
            return evolve(evolved, generations - skip);
        }
        
        void clear_memo() {
            memo_.clear();
        }
        
        size_t memo_size() const {
            return memo_.size();
        }
    };
};

// Example usage
int main() {
    Hashlife::HashlifeSimulator simulator;
    
    // Create simple pattern (glider)
    auto nw = Hashlife::QuadNode::create_leaf(false);
    auto ne = Hashlife::QuadNode::create_leaf(true);
    auto sw = Hashlife::QuadNode::create_leaf(true);
    auto se = Hashlife::QuadNode::create_leaf(true);
    
    auto pattern = Hashlife::QuadNode::create_node(nw, ne, sw, se);
    
    std::cout << "Hashlife simulator initialized" << std::endl;
    std::cout << "Memo size: " << simulator.memo_size() << std::endl;
    
    return 0;
}

