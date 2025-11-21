/*
 * PostgreSQL Query Planner Dynamic Programming
 *
 * Source: PostgreSQL source code, query optimization
 * Repository: https://github.com/postgres/postgres
 * Files: src/backend/optimizer/path/*.c, src/backend/optimizer/util/*.c
 * Algorithm: DP-based join order optimization with cost estimation
 *
 * What Makes It Ingenious:
 * - Dynamic programming for join order enumeration
 * - Cost-based optimization with multiple cost metrics
 * - Memoization of subquery results
 * - Pruning of suboptimal plans
 * - Integration with statistics and cost models
 * - Used in production database query optimization
 *
 * When to Use:
 * - Query optimization in databases
 * - Join order selection
 * - Cost-based plan enumeration
 * - Multi-table query planning
 * - Resource allocation with constraints
 * - Complex optimization problems with multiple objectives
 *
 * Real-World Usage:
 * - PostgreSQL query planner
 * - MySQL optimizer
 * - SQL Server query optimizer
 * - Oracle cost-based optimizer
 * - Database query planning systems
 *
 * Time Complexity: Exponential in number of tables (with pruning)
 * Space Complexity: O(2^n) for memoization table
 */

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#include <iostream>
#include <algorithm>
#include <limits>
#include <cmath>

// Forward declarations
struct QueryPlan;
struct TableInfo;

// Table information (simplified from PostgreSQL's RelOptInfo)
struct TableInfo {
    int id;
    std::string name;
    long long row_count;      // Number of rows
    double selectivity;       // Selectivity factor
    double cost_startup;      // Startup cost
    double cost_per_row;      // Cost per row
    std::vector<int> join_conditions; // Tables it can join with

    TableInfo(int _id, const std::string& _name, long long rows,
              double sel = 1.0, double startup = 0.0, double per_row = 1.0)
        : id(_id), name(_name), row_count(rows), selectivity(sel),
          cost_startup(startup), cost_per_row(per_row) {}
};

// Query plan node (simplified from PostgreSQL's Path)
struct QueryPlan {
    std::unordered_set<int> tables;     // Set of tables in this plan
    double total_cost;                  // Total execution cost
    long long estimated_rows;           // Estimated result rows
    double selectivity;                 // Combined selectivity
    std::vector<QueryPlan*> children;   // Child plans (for joins)
    enum JoinType { NESTED_LOOP, HASH_JOIN, MERGE_JOIN } join_type;

    QueryPlan() : total_cost(0.0), estimated_rows(0), selectivity(1.0),
                  join_type(NESTED_LOOP) {}

    // Calculate estimated cost and rows
    void calculate_estimates(const std::vector<TableInfo>& tables_info) {
        if (children.empty()) {
            // Base case: single table
            if (!tables.empty()) {
                int table_id = *tables.begin();
                const auto& info = tables_info[table_id];
                total_cost = info.cost_startup + info.row_count * info.cost_per_row;
                estimated_rows = info.row_count * info.selectivity;
                selectivity = info.selectivity;
            }
            return;
        }

        // Join case: combine children
        double cost = 0.0;
        long long rows = 1;
        double sel = 1.0;

        for (auto* child : children) {
            cost += child->total_cost;
            rows = static_cast<long long>(rows * child->estimated_rows);
            sel *= child->selectivity;
        }

        // Add join cost (simplified)
        cost += rows * 0.01; // Simplified join cost model

        total_cost = cost;
        estimated_rows = static_cast<long long>(rows * sel);
        selectivity = sel;
    }

    void print_plan(int indent = 0) const {
        std::string prefix(indent, ' ');
        std::cout << prefix << "Plan: cost=" << total_cost
                  << ", rows=" << estimated_rows << ", tables={";
        for (int t : tables) {
            std::cout << t << ",";
        }
        std::cout << "}" << std::endl;

        for (auto* child : children) {
            child->print_plan(indent + 2);
        }
    }
};

// PostgreSQL-style DP query planner
class PostgreSQLQueryPlanner {
private:
    std::vector<TableInfo> tables_;
    std::unordered_map<std::string, std::unique_ptr<QueryPlan>> memo_;
    int num_tables_;

    // Generate memo key for a set of tables
    std::string make_key(const std::unordered_set<int>& table_set) {
        std::vector<int> sorted_tables(table_set.begin(), table_set.end());
        std::sort(sorted_tables.begin(), sorted_tables.end());

        std::string key;
        for (size_t i = 0; i < sorted_tables.size(); ++i) {
            if (i > 0) key += ",";
            key += std::to_string(sorted_tables[i]);
        }
        return key;
    }

    // Check if two table sets can be joined
    bool can_join(const std::unordered_set<int>& left,
                  const std::unordered_set<int>& right) {
        // Check if any table in left can join with any table in right
        for (int l : left) {
            for (int r : right) {
                const auto& l_info = tables_[l];
                if (std::find(l_info.join_conditions.begin(),
                             l_info.join_conditions.end(), r) !=
                    l_info.join_conditions.end()) {
                    return true;
                }
            }
        }
        return false;
    }

    // Recursive DP function to find optimal plan for table set
    QueryPlan* find_optimal_plan(const std::unordered_set<int>& table_set) {
        if (table_set.empty()) return nullptr;

        std::string key = make_key(table_set);
        if (memo_.find(key) != memo_.end()) {
            return memo_[key].get();
        }

        // Base case: single table
        if (table_set.size() == 1) {
            int table_id = *table_set.begin();
            auto plan = std::make_unique<QueryPlan>();
            plan->tables = table_set;
            plan->calculate_estimates(tables_);
            memo_[key] = std::move(plan);
            return memo_[key].get();
        }

        // Try all possible ways to split the table set
        QueryPlan* best_plan = nullptr;
        double best_cost = std::numeric_limits<double>::max();

        // Generate all possible splits
        std::vector<int> table_list(table_set.begin(), table_set.end());

        // Try all possible left subsets (except empty and full)
        int n = table_list.size();
        for (int mask = 1; mask < (1 << n) - 1; ++mask) {
            std::unordered_set<int> left_set;
            std::unordered_set<int> right_set;

            for (int i = 0; i < n; ++i) {
                if (mask & (1 << i)) {
                    left_set.insert(table_list[i]);
                } else {
                    right_set.insert(table_list[i]);
                }
            }

            // Check if this split is valid (can join)
            if (!can_join(left_set, right_set)) {
                continue;
            }

            // Recursively find optimal plans for subsets
            QueryPlan* left_plan = find_optimal_plan(left_set);
            QueryPlan* right_plan = find_optimal_plan(right_set);

            if (!left_plan || !right_plan) continue;

            // Create join plan
            auto join_plan = std::make_unique<QueryPlan>();
            join_plan->tables = table_set;
            join_plan->children = {left_plan, right_plan};
            join_plan->calculate_estimates(tables_);

            // Add join cost based on join type selection
            double join_cost = estimate_join_cost(left_plan, right_plan);
            join_plan->total_cost += join_cost;

            // Keep track of best plan
            if (join_plan->total_cost < best_cost) {
                best_cost = join_plan->total_cost;
                best_plan = join_plan.get();
                memo_[key] = std::move(join_plan);
            }
        }

        return best_plan;
    }

    // Estimate join cost (simplified from PostgreSQL)
    double estimate_join_cost(QueryPlan* left, QueryPlan* right) {
        // Simplified cost model
        long long left_rows = left->estimated_rows;
        long long right_rows = right->estimated_rows;

        // Nested loop join cost
        double nested_loop_cost = left->total_cost + right->total_cost +
                                 left_rows * right_rows * 0.001;

        // Hash join cost (if one side fits in memory)
        double hash_join_cost = left->total_cost + right->total_cost +
                               std::max(left_rows, right_rows) * 0.01;

        // Merge join cost (if sorted)
        double merge_join_cost = left->total_cost + right->total_cost +
                                (left_rows + right_rows) * 0.005;

        return std::min({nested_loop_cost, hash_join_cost, merge_join_cost});
    }

public:
    PostgreSQLQueryPlanner(const std::vector<TableInfo>& tables)
        : tables_(tables), num_tables_(tables.size()) {}

    // Find optimal query plan using DP
    QueryPlan* optimize_query() {
        std::unordered_set<int> all_tables;
        for (int i = 0; i < num_tables_; ++i) {
            all_tables.insert(i);
        }

        return find_optimal_plan(all_tables);
    }

    // Print memo table statistics
    void print_statistics() const {
        std::cout << "Memo table size: " << memo_.size() << " entries" << std::endl;

        // Find most expensive plan
        QueryPlan* most_expensive = nullptr;
        for (const auto& pair : memo_) {
            if (!most_expensive ||
                pair.second->total_cost > most_expensive->total_cost) {
                most_expensive = pair.second.get();
            }
        }

        if (most_expensive) {
            std::cout << "Most expensive partial plan cost: "
                      << most_expensive->total_cost << std::endl;
        }
    }
};

// Demonstration
void demonstrate_postgresql_dp() {
    std::cout << "PostgreSQL Query Planner DP Demonstration:" << std::endl;

    // Create sample tables
    std::vector<TableInfo> tables = {
        {0, "customers", 10000, 1.0, 10.0, 0.1},
        {1, "orders", 50000, 1.0, 20.0, 0.05},
        {2, "products", 1000, 1.0, 5.0, 0.5},
        {3, "order_items", 150000, 1.0, 50.0, 0.02}
    };

    // Set up join conditions
    tables[0].join_conditions = {1};        // customers -> orders
    tables[1].join_conditions = {0, 3};     // orders -> customers, order_items
    tables[2].join_conditions = {3};        // products -> order_items
    tables[3].join_conditions = {1, 2};     // order_items -> orders, products

    PostgreSQLQueryPlanner planner(tables);
    QueryPlan* optimal_plan = planner.optimize_query();

    if (optimal_plan) {
        std::cout << "\nOptimal Query Plan:" << std::endl;
        optimal_plan->print_plan();
    }

    planner.print_statistics();

    // Demonstrate different join orders
    std::cout << "\nDP explores all possible join orders and selects the optimal one!" << std::endl;
    std::cout << "This is similar to how PostgreSQL uses DP for query optimization." << std::endl;
}

// Example usage
int main() {
    demonstrate_postgresql_dp();
    return 0;
}

