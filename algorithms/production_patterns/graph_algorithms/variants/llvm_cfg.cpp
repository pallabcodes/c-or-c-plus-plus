/*
 * LLVM Control Flow Graph - CFG Construction and Analysis
 * 
 * Source: https://github.com/llvm/llvm-project/blob/main/llvm/lib/Analysis/
 * Repository: llvm/llvm-project
 * Directory: `llvm/lib/Analysis/`
 * 
 * What Makes It Ingenious:
 * - Control flow graph construction from code
 * - Dominator tree computation
 * - Data flow analysis framework
 * - Compiler optimization patterns
 * - Efficient graph algorithms for compiler use
 * 
 * When to Use:
 * - Compiler construction
 * - Code analysis
 * - Optimization passes
 * - Static analysis
 * - Program understanding
 * 
 * Real-World Usage:
 * - LLVM compiler optimizations
 * - Static code analysis
 * - Compiler backends
 * - Code transformation tools
 * 
 * Time Complexity:
 * - CFG Construction: O(n) where n is basic blocks
 * - Dominator Tree: O(n log n) or better
 * - Data Flow Analysis: O(n * iterations)
 * 
 * Space Complexity: O(n + e) where n is nodes, e is edges
 */

#include <vector>
#include <map>
#include <set>
#include <queue>

// Basic block (simplified)
struct BasicBlock {
    int id;
    std::vector<int> instructions;
    std::vector<int> successors; // Outgoing edges
    std::vector<int> predecessors; // Incoming edges
    
    BasicBlock(int i) : id(i) {}
};

// Control Flow Graph
class ControlFlowGraph {
private:
    std::vector<BasicBlock*> blocks;
    int entry_block_id;
    
public:
    ControlFlowGraph() : entry_block_id(0) {}
    
    ~ControlFlowGraph() {
        for (BasicBlock* block : blocks) {
            delete block;
        }
    }
    
    // Add basic block
    void add_block(BasicBlock* block) {
        blocks.push_back(block);
    }
    
    // Add edge (control flow)
    void add_edge(int from_id, int to_id) {
        BasicBlock* from = get_block(from_id);
        BasicBlock* to = get_block(to_id);
        
        if (from && to) {
            from->successors.push_back(to_id);
            to->predecessors.push_back(from_id);
        }
    }
    
    // Get block by ID
    BasicBlock* get_block(int id) {
        for (BasicBlock* block : blocks) {
            if (block->id == id) {
                return block;
            }
        }
        return nullptr;
    }
    
    // Get entry block
    BasicBlock* get_entry() {
        return get_block(entry_block_id);
    }
    
    // Set entry block
    void set_entry(int id) {
        entry_block_id = id;
    }
    
    // Get all blocks
    const std::vector<BasicBlock*>& get_blocks() const {
        return blocks;
    }
    
    // Depth-first search
    void dfs(int block_id, std::set<int>& visited, 
             std::function<void(BasicBlock*)> visit) {
        if (visited.count(block_id)) return;
        
        visited.insert(block_id);
        BasicBlock* block = get_block(block_id);
        if (block) {
            visit(block);
            
            for (int succ_id : block->successors) {
                dfs(succ_id, visited, visit);
            }
        }
    }
    
    // Breadth-first search
    void bfs(int start_id, std::function<void(BasicBlock*)> visit) {
        std::set<int> visited;
        std::queue<int> queue;
        
        queue.push(start_id);
        visited.insert(start_id);
        
        while (!queue.empty()) {
            int block_id = queue.front();
            queue.pop();
            
            BasicBlock* block = get_block(block_id);
            if (block) {
                visit(block);
                
                for (int succ_id : block->successors) {
                    if (!visited.count(succ_id)) {
                        visited.insert(succ_id);
                        queue.push(succ_id);
                    }
                }
            }
        }
    }
};

// Dominator tree (simplified)
class DominatorTree {
private:
    ControlFlowGraph* cfg;
    std::map<int, int> idom; // Immediate dominator map
    
    // Compute immediate dominators (simplified algorithm)
    void compute_dominators() {
        BasicBlock* entry = cfg->get_entry();
        if (!entry) return;
        
        // Initialize: entry dominates itself
        idom[entry->id] = entry->id;
        
        // Initialize all other blocks
        for (BasicBlock* block : cfg->get_blocks()) {
            if (block->id != entry->id) {
                idom[block->id] = -1; // Unknown
            }
        }
        
        // Iterative data flow analysis
        bool changed = true;
        while (changed) {
            changed = false;
            
            for (BasicBlock* block : cfg->get_blocks()) {
                if (block->id == entry->id) continue;
                
                // Find intersection of dominators of predecessors
                int new_idom = -1;
                bool first = true;
                
                for (int pred_id : block->predecessors) {
                    if (idom[pred_id] != -1) {
                        if (first) {
                            new_idom = pred_id;
                            first = false;
                        } else {
                            new_idom = intersect(new_idom, pred_id);
                        }
                    }
                }
                
                if (new_idom != idom[block->id]) {
                    idom[block->id] = new_idom;
                    changed = true;
                }
            }
        }
    }
    
    // Find intersection of dominator chains
    int intersect(int b1, int b2) {
        while (b1 != b2) {
            while (b1 < b2) {
                b1 = idom[b1];
            }
            while (b2 < b1) {
                b2 = idom[b2];
            }
        }
        return b1;
    }
    
public:
    DominatorTree(ControlFlowGraph* c) : cfg(c) {
        compute_dominators();
    }
    
    // Get immediate dominator
    int get_idom(int block_id) {
        auto it = idom.find(block_id);
        return (it != idom.end()) ? it->second : -1;
    }
    
    // Check if block1 dominates block2
    bool dominates(int block1_id, int block2_id) {
        int current = block2_id;
        while (current != -1) {
            if (current == block1_id) {
                return true;
            }
            current = get_idom(current);
        }
        return false;
    }
};

// Example usage
#include <iostream>

int main() {
    // Create CFG
    ControlFlowGraph cfg;
    
    BasicBlock* entry = new BasicBlock(0);
    BasicBlock* block1 = new BasicBlock(1);
    BasicBlock* block2 = new BasicBlock(2);
    BasicBlock* block3 = new BasicBlock(3);
    BasicBlock* exit = new BasicBlock(4);
    
    cfg.add_block(entry);
    cfg.add_block(block1);
    cfg.add_block(block2);
    cfg.add_block(block3);
    cfg.add_block(exit);
    
    cfg.set_entry(0);
    
    // Add edges
    cfg.add_edge(0, 1);
    cfg.add_edge(1, 2);
    cfg.add_edge(1, 3);
    cfg.add_edge(2, 4);
    cfg.add_edge(3, 4);
    
    // DFS traversal
    std::cout << "DFS traversal:" << std::endl;
    std::set<int> visited;
    cfg.dfs(0, visited, [](BasicBlock* block) {
        std::cout << "Block " << block->id << std::endl;
    });
    
    // BFS traversal
    std::cout << "\nBFS traversal:" << std::endl;
    cfg.bfs(0, [](BasicBlock* block) {
        std::cout << "Block " << block->id << std::endl;
    });
    
    // Dominator tree
    DominatorTree dom_tree(&cfg);
    std::cout << "\nDominator tree:" << std::endl;
    for (BasicBlock* block : cfg.get_blocks()) {
        int idom = dom_tree.get_idom(block->id);
        std::cout << "Block " << block->id 
                  << " dominated by " << idom << std::endl;
    }
    
    return 0;
}

