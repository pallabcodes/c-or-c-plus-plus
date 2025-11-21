/*
 * Database Range Query Merging
 *
 * Source: PostgreSQL query optimizer, database index management
 * Repository: https://github.com/postgres/postgres
 * Files: src/backend/optimizer/path/indxpath.c, src/backend/access/nbtree/
 * Algorithm: B-tree index range consolidation and query optimization
 *
 * What Makes It Ingenious:
 * - B-tree integration: Merges ranges within index structures
 * - Query plan optimization: Consolidates multiple range predicates
 * - Cost-based merging: Considers I/O costs and selectivity
 * - Index-only scans: Avoids heap access for covered queries
 * - Used in PostgreSQL, MySQL, and other RDBMS optimizers
 * - Handles complex predicates and multiple indexes
 *
 * When to Use:
 * - Database query optimization
 * - Index range scans
 * - Multiple range predicates
 * - B-tree index maintenance
 * - Query plan generation
 * - OLAP query processing
 *
 * Real-World Usage:
 * - PostgreSQL query planner
 * - MySQL optimizer
 * - SQL Server execution plans
 * - Database index operations
 * - Range query processing
 * - Data warehouse optimization
 *
 * Time Complexity: O(n log n) for sorting ranges, O(n) for merging
 * Space Complexity: O(n) for storing merged ranges
 */

#include <vector>
#include <set>
#include <memory>
#include <functional>
#include <iostream>
#include <algorithm>
#include <limits>

// Database range predicate representation
template<typename T>
struct RangePredicate {
    T lower_bound;
    T upper_bound;
    bool lower_inclusive;  // true for >=, false for >
    bool upper_inclusive;  // true for <=, false for <
    bool is_null_allowed;  // Allow NULL values
    double selectivity;    // Estimated fraction of rows matching

    RangePredicate(T low = T{}, T high = T{},
                  bool low_inc = true, bool high_inc = true,
                  bool null_allowed = true, double sel = 1.0)
        : lower_bound(low), upper_bound(high),
          lower_inclusive(low_inc), upper_inclusive(high_inc),
          is_null_allowed(null_allowed), selectivity(sel) {}

    // Check if this predicate matches a value
    bool matches(const T& value) const {
        if (value < lower_bound || value > upper_bound) return false;
        if (!lower_inclusive && value == lower_bound) return false;
        if (!upper_inclusive && value == upper_bound) return false;
        return true;
    }

    // Check if ranges overlap
    bool overlaps(const RangePredicate& other) const {
        // Two ranges overlap if they are not disjoint
        T left_max = std::max(lower_bound, other.lower_bound);
        T right_min = std::min(upper_bound, other.upper_bound);

        if (left_max <= right_min) {
            // They overlap in the continuous case
            // But we need to consider inclusivity
            if (left_max == right_min) {
                // They touch at a point - check inclusivity
                bool left_inclusive = (left_max == lower_bound) ? lower_inclusive :
                                    (left_max == other.lower_bound) ? other.lower_inclusive : true;
                bool right_inclusive = (right_min == upper_bound) ? upper_inclusive :
                                     (right_min == other.upper_bound) ? other.upper_inclusive : true;
                return left_inclusive && right_inclusive;
            }
            return true;
        }
        return false;
    }

    // Merge two overlapping ranges
    RangePredicate merge(const RangePredicate& other) const {
        // Take the union of the ranges
        T new_lower = std::min(lower_bound, other.lower_bound);
        T new_upper = std::max(upper_bound, other.upper_bound);

        // Determine inclusivity at boundaries
        bool new_lower_inc = (new_lower == lower_bound) ? lower_inclusive :
                           (new_lower == other.lower_bound) ? other.lower_inclusive : true;
        bool new_upper_inc = (new_upper == upper_bound) ? upper_inclusive :
                           (new_upper == other.upper_bound) ? other.upper_inclusive : true;

        // Combine selectivity (approximation)
        double new_selectivity = std::min(1.0, selectivity + other.selectivity);

        return RangePredicate(new_lower, new_upper, new_lower_inc, new_upper_inc,
                            is_null_allowed && other.is_null_allowed, new_selectivity);
    }

    void print() const {
        std::cout << (lower_inclusive ? "[" : "(") << lower_bound << ", "
                  << upper_bound << (upper_inclusive ? "]" : ")")
                  << " sel=" << selectivity;
        if (is_null_allowed) std::cout << " NULL";
    }
};

// B-tree index range representation
template<typename T>
struct BTreeRange {
    T key_start;
    T key_end;
    size_t block_count;     // Number of disk blocks covered
    double cost;           // I/O cost estimate
    bool is_unique;        // Unique index or not

    BTreeRange(T start, T end, size_t blocks = 1, double io_cost = 1.0, bool unique = false)
        : key_start(start), key_end(end), block_count(blocks),
          cost(io_cost), is_unique(unique) {}

    // Check if ranges are adjacent in B-tree
    bool adjacent(const BTreeRange& other) const {
        return key_end == other.key_start || other.key_end == key_start;
    }

    // Merge adjacent ranges
    BTreeRange merge(const BTreeRange& other) const {
        T new_start = std::min(key_start, other.key_start);
        T new_end = std::max(key_end, other.key_end);
        size_t new_blocks = block_count + other.block_count;
        double new_cost = cost + other.cost;
        bool new_unique = is_unique && other.is_unique;

        return BTreeRange(new_start, new_end, new_blocks, new_cost, new_unique);
    }

    void print() const {
        std::cout << "B-tree range [" << key_start << ", " << key_end << "] "
                  << block_count << " blocks, cost=" << cost
                  << (is_unique ? " UNIQUE" : "");
    }
};

// PostgreSQL-style range query optimizer
template<typename T>
class DatabaseRangeOptimizer {
private:
    std::vector<RangePredicate<T>> predicates_;
    std::vector<BTreeRange<T>> index_ranges_;

    // Cost model constants (simplified)
    static constexpr double SEQ_PAGE_COST = 1.0;
    static constexpr double INDEX_PAGE_COST = 0.1;
    static constexpr double CPU_OPERATOR_COST = 0.0025;

public:
    // Add a range predicate from WHERE clause
    void add_predicate(const RangePredicate<T>& pred) {
        predicates_.push_back(pred);
    }

    // Add an index range
    void add_index_range(const BTreeRange<T>& range) {
        index_ranges_.push_back(range);
    }

    // Optimize range predicates by merging overlapping ones
    std::vector<RangePredicate<T>> optimize_predicates() {
        if (predicates_.empty()) return {};

        // Sort predicates by lower bound
        std::sort(predicates_.begin(), predicates_.end(),
                 [](const RangePredicate<T>& a, const RangePredicate<T>& b) {
                     return a.lower_bound < b.lower_bound;
                 });

        std::vector<RangePredicate<T>> optimized;
        optimized.push_back(predicates_[0]);

        for (size_t i = 1; i < predicates_.size(); ++i) {
            auto& last = optimized.back();
            const auto& current = predicates_[i];

            if (last.overlaps(current)) {
                // Merge overlapping predicates
                last = last.merge(current);
            } else {
                // Add as separate predicate
                optimized.push_back(current);
            }
        }

        return optimized;
    }

    // Optimize index ranges for sequential I/O
    std::vector<BTreeRange<T>> optimize_index_ranges() {
        if (index_ranges_.empty()) return {};

        // Sort by key start
        std::sort(index_ranges_.begin(), index_ranges_.end(),
                 [](const BTreeRange<T>& a, const BTreeRange<T>& b) {
                     return a.key_start < b.key_start;
                 });

        std::vector<BTreeRange<T>> optimized;
        optimized.push_back(index_ranges_[0]);

        for (size_t i = 1; i < index_ranges_.size(); ++i) {
            auto& last = optimized.back();
            const auto& current = index_ranges_[i];

            if (last.adjacent(current) ||
                (last.key_end >= current.key_start && current.key_end >= last.key_start)) {
                // Merge overlapping or adjacent ranges
                last = last.merge(current);
            } else {
                optimized.push_back(current);
            }
        }

        return optimized;
    }

    // Estimate query cost for given ranges
    double estimate_query_cost(const std::vector<RangePredicate<T>>& predicates,
                              const std::vector<BTreeRange<T>>& index_ranges) {
        double total_cost = 0.0;

        // Index scan cost
        for (const auto& range : index_ranges) {
            total_cost += range.cost * INDEX_PAGE_COST;
        }

        // CPU cost for predicate evaluation
        for (const auto& pred : predicates) {
            // Estimate rows that need CPU processing
            double rows_processed = pred.selectivity * 10000; // Assume 10k rows
            total_cost += rows_processed * CPU_OPERATOR_COST;
        }

        return total_cost;
    }

    // Find optimal query plan
    struct QueryPlan {
        std::vector<RangePredicate<T>> predicates;
        std::vector<BTreeRange<T>> index_ranges;
        double estimated_cost;
        std::string strategy; // "index_scan", "seq_scan", "bitmap_scan"

        void print() const {
            std::cout << "Query Plan (" << strategy << "):" << std::endl;
            std::cout << "  Cost: " << estimated_cost << std::endl;
            std::cout << "  Predicates:" << std::endl;
            for (const auto& pred : predicates) {
                std::cout << "    ";
                pred.print();
                std::cout << std::endl;
            }
            std::cout << "  Index ranges:" << std::endl;
            for (const auto& range : index_ranges) {
                std::cout << "    ";
                range.print();
                std::cout << std::endl;
            }
        }
    };

    QueryPlan find_optimal_plan() {
        // Optimize predicates and index ranges
        auto opt_predicates = optimize_predicates();
        auto opt_ranges = optimize_index_ranges();

        // Estimate costs
        double index_cost = estimate_query_cost(opt_predicates, opt_ranges);
        double seq_cost = 10000 * SEQ_PAGE_COST; // Sequential scan cost

        // Choose strategy
        std::string strategy;
        double cost;

        if (index_cost < seq_cost && !opt_ranges.empty()) {
            strategy = "index_scan";
            cost = index_cost;
        } else if (opt_predicates.size() == 1 && opt_predicates[0].selectivity < 0.1) {
            strategy = "bitmap_scan";
            cost = index_cost * 1.2; // Bitmap scan has some overhead
        } else {
            strategy = "seq_scan";
            cost = seq_cost;
        }

        return QueryPlan{opt_predicates, opt_ranges, cost, strategy};
    }

    // Clear all predicates and ranges
    void clear() {
        predicates_.clear();
        index_ranges_.clear();
    }
};

// Example usage
int main() {
    std::cout << "Database Range Query Merging Demonstration:" << std::endl;

    DatabaseRangeOptimizer<int> optimizer;

    // Add some range predicates (like WHERE clauses)
    optimizer.add_predicate(RangePredicate<int>(10, 50, true, true, false, 0.4));
    optimizer.add_predicate(RangePredicate<int>(30, 70, true, false, false, 0.3));
    optimizer.add_predicate(RangePredicate<int>(80, 120, false, true, false, 0.2));
    optimizer.add_predicate(RangePredicate<int>(100, 150, true, true, false, 0.25));

    // Add index ranges
    optimizer.add_index_range(BTreeRange<int>(10, 50, 5, 2.5, false));
    optimizer.add_index_range(BTreeRange<int>(30, 70, 4, 2.0, false));
    optimizer.add_index_range(BTreeRange<int>(80, 120, 3, 1.5, true));

    std::cout << "Original predicates:" << std::endl;
    for (const auto& pred : optimizer.optimize_predicates()) {
        std::cout << "  ";
        pred.print();
        std::cout << std::endl;
    }

    std::cout << "\nOptimized query plan:" << std::endl;
    auto plan = optimizer.find_optimal_plan();
    plan.print();

    return 0;
}

