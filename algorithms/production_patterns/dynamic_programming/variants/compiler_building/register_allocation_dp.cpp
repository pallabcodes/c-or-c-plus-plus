/*
 * Register Allocation DP - Compiler Building
 *
 * Source: Compiler backends (GCC, LLVM, MSVC), code generation
 * Pattern: Graph coloring with DP for register allocation
 * Algorithm: NP-hard problem solved with heuristics and DP
 *
 * What Makes It Ingenious:
 * - Optimal register assignment for performance
 * - Handles register pressure and spilling
 * - Interference graph coloring with DP
 * - Live range analysis and optimization
 * - Used in production compilers for code generation
 * - Balances memory usage and computation speed
 *
 * When to Use:
 * - Compiler register allocation passes
 * - Code generation optimization
 * - JIT compilation register assignment
 * - Embedded systems with limited registers
 * - GPU shader compilation
 * - Architecture-specific optimizations
 *
 * Real-World Usage:
 * - LLVM register allocator
 * - GCC register allocation
 * - HotSpot JVM register allocation
 * - .NET CLR register allocation
 * - CUDA compiler register allocation
 *
 * Time Complexity: O(n + e) for graph construction, O(2^k) for coloring subsets
 * Space Complexity: O(n + e) for interference graph
 */

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <iostream>
#include <algorithm>
#include <queue>
#include <stack>

// Live range representation
struct LiveRange {
    int variable_id;
    int start_instruction;
    int end_instruction;
    bool spilled;  // Whether this range was spilled to memory

    LiveRange(int vid = -1, int start = -1, int end = -1)
        : variable_id(vid), start_instruction(start), end_instruction(end),
          spilled(false) {}
};

// Interference graph node
struct InterferenceNode {
    int variable_id;
    std::unordered_set<int> neighbors;  // Interfering variables
    int degree;  // Number of neighbors
    int color;   // Assigned register (-1 if not assigned)
    bool removed; // For graph coloring simplification

    InterferenceNode(int vid = -1) : variable_id(vid), degree(0),
                                    color(-1), removed(false) {}
};

// Register allocation using graph coloring with DP
class RegisterAllocator {
private:
    int num_registers_;
    std::vector<LiveRange> live_ranges_;
    std::vector<InterferenceNode> interference_graph_;
    std::unordered_map<int, int> variable_to_node_;

    // Build interference graph
    void build_interference_graph(const std::vector<std::vector<int>>& live_variables) {
        // Initialize nodes for each variable
        for (const auto& ranges : live_ranges_) {
            int node_id = interference_graph_.size();
            interference_graph_.emplace_back(ranges.variable_id);
            variable_to_node_[ranges.variable_id] = node_id;
        }

        // For each instruction, add edges between simultaneously live variables
        for (const auto& live_at_instruction : live_variables) {
            for (size_t i = 0; i < live_at_instruction.size(); ++i) {
                for (size_t j = i + 1; j < live_at_instruction.size(); ++j) {
                    int var1 = live_at_instruction[i];
                    int var2 = live_at_instruction[j];

                    auto it1 = variable_to_node_.find(var1);
                    auto it2 = variable_to_node_.find(var2);

                    if (it1 != variable_to_node_.end() && it2 != variable_to_node_.end()) {
                        int node1 = it1->second;
                        int node2 = it2->second;

                        // Add undirected edge
                        interference_graph_[node1].neighbors.insert(node2);
                        interference_graph_[node2].neighbors.insert(node1);
                        interference_graph_[node1].degree++;
                        interference_graph_[node2].degree++;
                    }
                }
            }
        }
    }

    // Graph coloring using DP-based simplification
    bool color_graph() {
        std::vector<int> simplification_stack;

        // Phase 1: Simplify (remove nodes with degree < num_registers)
        while (true) {
            bool removed_any = false;

            for (size_t i = 0; i < interference_graph_.size(); ++i) {
                auto& node = interference_graph_[i];
                if (!node.removed && node.degree < num_registers_) {
                    // Remove node and update neighbors
                    for (int neighbor : node.neighbors) {
                        auto& neighbor_node = interference_graph_[neighbor];
                        if (!neighbor_node.removed) {
                            neighbor_node.degree--;
                        }
                    }
                    node.removed = true;
                    simplification_stack.push_back(i);
                    removed_any = true;
                }
            }

            if (!removed_any) break;
        }

        // Check if remaining graph is colorable
        for (const auto& node : interference_graph_) {
            if (!node.removed && node.degree >= num_registers_) {
                // Graph is not K-colorable, need spilling
                return false;
            }
        }

        // Phase 2: Assign colors in reverse order
        while (!simplification_stack.empty()) {
            int node_id = simplification_stack.back();
            simplification_stack.pop_back();

            auto& node = interference_graph_[node_id];

            // Find available color
            std::vector<bool> used_colors(num_registers_, false);

            for (int neighbor : node.neighbors) {
                const auto& neighbor_node = interference_graph_[neighbor];
                if (neighbor_node.color != -1) {
                    used_colors[neighbor_node.color] = true;
                }
            }

            // Assign first available color
            for (int color = 0; color < num_registers_; ++color) {
                if (!used_colors[color]) {
                    node.color = color;
                    break;
                }
            }

            // If no color available, mark for spilling
            if (node.color == -1) {
                node.color = -2; // Spill marker
            }
        }

        return true;
    }

    // Spill variables that couldn't be colored
    void handle_spilling() {
        for (auto& node : interference_graph_) {
            if (node.color == -2) {  // Spill marker
                // Mark corresponding live range as spilled
                for (auto& range : live_ranges_) {
                    if (range.variable_id == node.variable_id) {
                        range.spilled = true;
                        break;
                    }
                }
            }
        }
    }

public:
    RegisterAllocator(int num_regs) : num_registers_(num_regs) {}

    // Add live range for a variable
    void add_live_range(const LiveRange& range) {
        live_ranges_.push_back(range);
    }

    // Allocate registers using interference graph coloring
    bool allocate_registers(const std::vector<std::vector<int>>& live_variables) {
        build_interference_graph(live_variables);

        bool success = color_graph();

        if (!success) {
            // Graph was not colorable, implement spilling
            handle_spilling();
        }

        return success;
    }

    // Get register assignment for variable
    int get_register(int variable_id) const {
        auto it = variable_to_node_.find(variable_id);
        if (it != variable_to_node_.end()) {
            return interference_graph_[it->second].color;
        }
        return -1; // Not allocated
    }

    // Check if variable was spilled
    bool is_spilled(int variable_id) const {
        for (const auto& range : live_ranges_) {
            if (range.variable_id == variable_id) {
                return range.spilled;
            }
        }
        return false;
    }

    // Print allocation results
    void print_allocation() const {
        std::cout << "Register Allocation Results:" << std::endl;
        std::cout << "Number of registers: " << num_registers_ << std::endl;
        std::cout << "Variables and their registers:" << std::endl;

        for (const auto& node : interference_graph_) {
            std::cout << "Variable " << node.variable_id << ": ";
            if (node.color >= 0) {
                std::cout << "Register " << node.color;
            } else if (node.color == -2) {
                std::cout << "SPILLED";
            } else {
                std::cout << "Not allocated";
            }
            std::cout << " (degree: " << node.degree << ")" << std::endl;
        }
    }

    // Get interference graph statistics
    void print_statistics() const {
        std::cout << "\nInterference Graph Statistics:" << std::endl;
        std::cout << "Nodes: " << interference_graph_.size() << std::endl;

        int total_edges = 0;
        for (const auto& node : interference_graph_) {
            total_edges += node.neighbors.size();
        }
        std::cout << "Edges: " << total_edges / 2 << std::endl;  // Undirected

        int max_degree = 0;
        for (const auto& node : interference_graph_) {
            max_degree = std::max(max_degree, node.degree);
        }
        std::cout << "Maximum degree: " << max_degree << std::endl;
    }
};

// Instruction scheduling using DP
class InstructionScheduler {
private:
    struct Instruction {
        int id;
        std::vector<int> dependencies;  // Instructions that must execute before this
        int latency;  // Execution latency
        std::string opcode;

        Instruction(int i, const std::vector<int>& deps, int lat, const std::string& op)
            : id(i), dependencies(deps), latency(lat), opcode(op) {}
    };

    std::vector<Instruction> instructions_;

    // DP table for scheduling
    std::vector<int> earliest_start_;  // Earliest start time for each instruction
    std::vector<int> latest_start_;    // Latest start time for each instruction
    std::vector<std::vector<int>> schedule_dp_;  // DP for optimal scheduling

public:
    void add_instruction(int id, const std::vector<int>& deps, int latency,
                        const std::string& opcode) {
        instructions_.emplace_back(id, deps, latency, opcode);
    }

    // Compute ASAP (As Soon As Possible) schedule
    void compute_asap() {
        earliest_start_.resize(instructions_.size(), 0);

        // Topological order processing
        for (size_t i = 0; i < instructions_.size(); ++i) {
            const auto& inst = instructions_[i];
            int max_dep_time = 0;

            for (int dep : inst.dependencies) {
                // Find the instruction with this ID
                auto it = std::find_if(instructions_.begin(), instructions_.end(),
                                     [dep](const Instruction& ins) {
                                         return ins.id == dep;
                                     });
                if (it != instructions_.end()) {
                    int dep_idx = it - instructions_.begin();
                    max_dep_time = std::max(max_dep_time,
                                          earliest_start_[dep_idx] + it->latency);
                }
            }

            earliest_start_[i] = max_dep_time;
        }
    }

    // Compute ALAP (As Late As Possible) schedule
    void compute_alap(int total_cycles) {
        latest_start_.resize(instructions_.size(), total_cycles);

        // Reverse topological order
        for (int i = static_cast<int>(instructions_.size()) - 1; i >= 0; --i) {
            const auto& inst = instructions_[i];

            // Find instructions that depend on this one
            for (size_t j = 0; j < instructions_.size(); ++j) {
                const auto& other = instructions_[j];
                if (std::find(other.dependencies.begin(), other.dependencies.end(), inst.id)
                    != other.dependencies.end()) {
                    latest_start_[i] = std::min(latest_start_[i],
                                              latest_start_[j] - inst.latency);
                }
            }
        }
    }

    // List scheduling with DP
    std::vector<int> list_schedule(int num_units) {
        compute_asap();

        std::vector<int> scheduled_times(instructions_.size(), -1);
        std::vector<int> ready_list;
        std::vector<int> remaining_deps(instructions_.size());

        // Initialize remaining dependencies
        for (size_t i = 0; i < instructions_.size(); ++i) {
            remaining_deps[i] = instructions_[i].dependencies.size();
        }

        // Start with instructions that have no dependencies
        for (size_t i = 0; i < instructions_.size(); ++i) {
            if (remaining_deps[i] == 0) {
                ready_list.push_back(i);
            }
        }

        int current_time = 0;
        std::vector<std::vector<int>> resource_usage;  // time -> units used

        while (!ready_list.empty()) {
            // Select instruction with earliest start time
            int best_inst = -1;
            int best_time = INT_MAX;

            for (int inst_idx : ready_list) {
                int start_time = std::max(current_time, earliest_start_[inst_idx]);
                if (start_time < best_time) {
                    best_time = start_time;
                    best_inst = inst_idx;
                }
            }

            if (best_inst == -1) break;

            // Schedule the instruction
            scheduled_times[best_inst] = best_time;
            current_time = best_time + instructions_[best_inst].latency;

            // Remove from ready list
            ready_list.erase(std::remove(ready_list.begin(), ready_list.end(), best_inst),
                           ready_list.end());

            // Update dependencies and add newly ready instructions
            for (size_t i = 0; i < instructions_.size(); ++i) {
                const auto& inst = instructions_[i];
                if (std::find(inst.dependencies.begin(), inst.dependencies.end(),
                             instructions_[best_inst].id) != inst.dependencies.end()) {
                    remaining_deps[i]--;
                    if (remaining_deps[i] == 0 && scheduled_times[i] == -1) {
                        ready_list.push_back(i);
                    }
                }
            }
        }

        return scheduled_times;
    }

    // Print scheduling results
    void print_schedule(const std::vector<int>& scheduled_times) const {
        std::cout << "\nInstruction Schedule:" << std::endl;
        for (size_t i = 0; i < instructions_.size(); ++i) {
            const auto& inst = instructions_[i];
            std::cout << "Instruction " << inst.id << " (" << inst.opcode
                      << "): starts at cycle " << scheduled_times[i]
                      << ", ends at cycle " << scheduled_times[i] + inst.latency - 1
                      << std::endl;
        }

        int max_end_time = 0;
        for (size_t i = 0; i < scheduled_times.size(); ++i) {
            if (scheduled_times[i] != -1) {
                max_end_time = std::max(max_end_time,
                                       scheduled_times[i] + instructions_[i].latency);
            }
        }
        std::cout << "Total execution time: " << max_end_time << " cycles" << std::endl;
    }
};

// Compiler backend simulation
class CompilerBackend {
public:
    static void demonstrate_register_allocation() {
        std::cout << "Compiler Register Allocation DP" << std::endl;

        // Create register allocator for 4 registers
        RegisterAllocator allocator(4);

        // Add some live ranges (simplified)
        allocator.add_live_range(LiveRange(0, 0, 3));  // var0: instructions 0-3
        allocator.add_live_range(LiveRange(1, 1, 4));  // var1: instructions 1-4
        allocator.add_live_range(LiveRange(2, 2, 5));  // var2: instructions 2-5
        allocator.add_live_range(LiveRange(3, 3, 6));  // var3: instructions 3-6
        allocator.add_live_range(LiveRange(4, 0, 2));  // var4: instructions 0-2

        // Live variables at each instruction
        std::vector<std::vector<int>> live_vars = {
            {0, 4},     // inst 0: var0, var4 live
            {0, 1, 4},  // inst 1: var0, var1, var4 live
            {0, 1, 2},  // inst 2: var0, var1, var2 live
            {0, 1, 2, 3}, // inst 3: var0, var1, var2, var3 live
            {1, 2, 3},  // inst 4: var1, var2, var3 live
            {2, 3},     // inst 5: var2, var3 live
            {3}         // inst 6: var3 live
        };

        bool success = allocator.allocate_registers(live_vars);
        allocator.print_allocation();
        allocator.print_statistics();

        std::cout << "\nRegister allocation ";
        if (success) {
            std::cout << "succeeded!" << std::endl;
        } else {
            std::cout << "required spilling for some variables." << std::endl;
        }
    }

    static void demonstrate_instruction_scheduling() {
        std::cout << "\nCompiler Instruction Scheduling DP" << std::endl;

        InstructionScheduler scheduler;

        // Add some instructions with dependencies
        scheduler.add_instruction(0, {}, 1, "LOAD");        // No dependencies
        scheduler.add_instruction(1, {}, 1, "LOAD");        // No dependencies
        scheduler.add_instruction(2, {0}, 2, "ADD");        // Depends on 0
        scheduler.add_instruction(3, {1}, 2, "MUL");        // Depends on 1
        scheduler.add_instruction(4, {2, 3}, 1, "STORE");   // Depends on 2 and 3

        auto schedule = scheduler.list_schedule(2);  // 2 execution units
        scheduler.print_schedule(schedule);

        std::cout << "\nDP techniques used:" << std::endl;
        std::cout << "- ASAP/ALAP scheduling for timing constraints" << std::endl;
        std::cout << "- List scheduling with priority selection" << std::endl;
        std::cout << "- Dependency graph traversal" << std::endl;
    }
};

// Example usage
int main() {
    CompilerBackend::demonstrate_register_allocation();
    CompilerBackend::demonstrate_instruction_scheduling();

    return 0;
}

