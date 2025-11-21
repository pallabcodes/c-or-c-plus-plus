/*
 * Persistent Segment Tree for Dynamic Programming Version Control
 *
 * Source: Functional data structures, competitive programming
 * Algorithm: Immutable segment tree with path copying for versions
 * Paper: Various persistent data structures research
 *
 * What Makes It Ingenious:
 * - Immutable: Updates create new versions without modifying old ones
 * - Path copying: Only O(log n) nodes copied per update
 * - Time travel: Access any previous version of the data
 * - Memory efficient: Shares common subtrees between versions
 * - Used for version-controlled DP states, undo operations
 *
 * When to Use:
 * - Need multiple versions of DP state
 * - Time-travel queries (what was the value at time t?)
 * - Undo operations in DP
 * - Functional programming style
 * - Competitive programming with multiple scenarios
 * - When you need to backtrack to previous states
 *
 * Real-World Usage:
 * - Version control systems
 * - Database transaction logs
 * - Undo/redo functionality
 * - Multi-version concurrency control
 * - Competitive programming problems
 * - Functional reactive programming
 *
 * Time Complexity:
 * - Update: O(log n)
 * - Query: O(log n)
 * - Space per update: O(log n)
 *
 * Space Complexity: O(n log n) worst case, O(n) amortized
 */

#include <vector>
#include <memory>
#include <functional>
#include <iostream>

template<typename T>
class PersistentSegmentTree {
private:
    struct Node {
        T value;
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;

        Node(T val = T{}) : value(val), left(nullptr), right(nullptr) {}
        Node(T val, std::shared_ptr<Node> l, std::shared_ptr<Node> r)
            : value(val), left(l), right(r) {}
    };

    int n;
    std::function<T(T, T)> combine_func;
    T identity;
    std::vector<std::shared_ptr<Node>> roots; // One root per version

    // Build initial tree
    std::shared_ptr<Node> build_tree(int start, int end) {
        if (start == end) {
            return std::make_shared<Node>(identity);
        }

        int mid = (start + end) / 2;
        auto left = build_tree(start, mid);
        auto right = build_tree(mid + 1, end);

        return std::make_shared<Node>(
            combine_func(left->value, right->value),
            left, right
        );
    }

    // Update tree at position, returning new root
    std::shared_ptr<Node> update_tree(std::shared_ptr<Node> node,
                                    int start, int end, int idx, T val) {
        if (start == end) {
            return std::make_shared<Node>(val);
        }

        int mid = (start + end) / 2;
        std::shared_ptr<Node> new_left = node->left;
        std::shared_ptr<Node> new_right = node->right;

        if (idx <= mid) {
            new_left = update_tree(node->left, start, mid, idx, val);
        } else {
            new_right = update_tree(node->right, mid + 1, end, idx, val);
        }

        T new_value = combine_func(
            new_left ? new_left->value : identity,
            new_right ? new_right->value : identity
        );

        return std::make_shared<Node>(new_value, new_left, new_right);
    }

    // Query range in tree
    T query_tree(std::shared_ptr<Node> node, int start, int end,
                int left, int right) {
        if (!node || right < start || end < left) {
            return identity;
        }

        if (left <= start && end <= right) {
            return node->value;
        }

        int mid = (start + end) / 2;
        T left_result = query_tree(node->left, start, mid, left, right);
        T right_result = query_tree(node->right, mid + 1, end, left, right);

        return combine_func(left_result, right_result);
    }

public:
    PersistentSegmentTree(int _n,
                         std::function<T(T, T)> combine,
                         T identity_elem = T{})
        : n(_n), combine_func(combine), identity(identity_elem) {

        // Build initial tree (version 0)
        roots.push_back(build_tree(0, n - 1));
    }

    // Update at position, create new version
    int update(int version, int idx, T val) {
        if (version < 0 || version >= (int)roots.size()) {
            return -1; // Invalid version
        }

        auto new_root = update_tree(roots[version], 0, n - 1, idx, val);
        roots.push_back(new_root);
        return roots.size() - 1; // Return new version number
    }

    // Query range in specific version
    T query(int version, int left, int right) {
        if (version < 0 || version >= (int)roots.size()) {
            return identity; // Invalid version
        }

        if (left > right || left < 0 || right >= n) {
            return identity;
        }

        return query_tree(roots[version], 0, n - 1, left, right);
    }

    // Get current number of versions
    size_t get_version_count() const {
        return roots.size();
    }

    // Get tree size (for debugging)
    size_t get_tree_size() const {
        return roots.size(); // Approximate
    }
};

// DP with Persistent Segment Trees
class PersistentDPSegmentTree {
public:
    // Example: DP with time travel - maximum subarray sum at different points
    static void dp_with_time_travel() {
        std::cout << "Persistent Segment Tree DP Demonstration:" << std::endl;

        int n = 10;
        PersistentSegmentTree<long long> pst(n, std::plus<long long>(), 0LL);

        std::vector<long long> arr = {1, -2, 3, -4, 5, -6, 7, -8, 9, -10};

        // Build DP table over time
        std::vector<int> versions;

        // Initial version (all zeros)
        versions.push_back(0);

        // Add elements one by one, maintaining maximum subarray ending at each position
        for (int i = 0; i < n; ++i) {
            int prev_version = versions.back();

            // Update DP[i] = max(arr[i], DP[i-1] + arr[i])
            long long prev_max = (i > 0) ? pst.query(prev_version, i-1, i-1) : 0;
            long long current_max = std::max(arr[i], prev_max + arr[i]);

            int new_version = pst.update(prev_version, i, current_max);
            versions.push_back(new_version);
        }

        // Query maximum subarray sum ending at each position for different versions
        std::cout << "\nMaximum subarray sum ending at each position:" << std::endl;
        for (int i = 0; i < n; ++i) {
            long long max_ending_here = pst.query(versions.back(), i, i);
            std::cout << "Position " << i << ": " << max_ending_here << std::endl;
        }

        // Time travel: What was the maximum at position 5 in earlier versions?
        std::cout << "\nTime travel - max at position 5 in different versions:" << std::endl;
        for (size_t v = 1; v < versions.size(); ++v) {
            long long val = pst.query(versions[v], 5, 5);
            std::cout << "Version " << v << ": " << val << std::endl;
        }
    }

    // Example: Persistent range sum queries for DP states
    static void persistent_range_sums() {
        std::cout << "\nPersistent Range Sum DP:" << std::endl;

        int n = 8;
        PersistentSegmentTree<long long> pst(n, std::plus<long long>(), 0LL);

        // Simulate DP where each version adds a new row
        std::vector<std::vector<long long>> dp_table = {
            {1, 2, 3, 4},
            {2, 3, 4, 5},
            {3, 4, 5, 6},
            {4, 5, 6, 7}
        };

        std::vector<int> versions;
        versions.push_back(0); // Initial empty version

        for (size_t row = 0; row < dp_table.size(); ++row) {
            int prev_version = versions.back();
            int new_version = prev_version;

            for (size_t col = 0; col < dp_table[row].size(); ++col) {
                size_t idx = row * dp_table[row].size() + col;
                new_version = pst.update(new_version, idx, dp_table[row][col]);
            }

            versions.push_back(new_version);
        }

        // Query entire DP table in different versions
        std::cout << "DP table sums in different versions:" << std::endl;
        for (size_t v = 1; v < versions.size(); ++v) {
            long long total_sum = pst.query(versions[v], 0, n-1);
            std::cout << "Version " << v << " total sum: " << total_sum << std::endl;
        }

        // Query specific ranges (e.g., row sums)
        std::cout << "\nRow sums in final version:" << std::endl;
        int row_size = dp_table[0].size();
        for (size_t row = 0; row < dp_table.size(); ++row) {
            int start = row * row_size;
            int end = start + row_size - 1;
            long long row_sum = pst.query(versions.back(), start, end);
            std::cout << "Row " << row << " sum: " << row_sum << std::endl;
        }
    }

    // Demonstrate undo functionality
    static void undo_functionality() {
        std::cout << "\nUndo Functionality with Persistent Trees:" << std::endl;

        int n = 5;
        PersistentSegmentTree<int> pst(n, [](int a, int b) { return a + b; }, 0);

        // Initial state: [0, 0, 0, 0, 0]
        std::vector<int> versions = {0};

        // Make updates, each creating a new version
        versions.push_back(pst.update(versions.back(), 0, 1));  // [1, 0, 0, 0, 0]
        versions.push_back(pst.update(versions.back(), 1, 2));  // [1, 2, 0, 0, 0]
        versions.push_back(pst.update(versions.back(), 2, 3));  // [1, 2, 3, 0, 0]
        versions.push_back(pst.update(versions.back(), 3, 4));  // [1, 2, 3, 4, 0]
        versions.push_back(pst.update(versions.back(), 4, 5));  // [1, 2, 3, 4, 5]

        // Query different versions (undo to previous states)
        for (int v = 0; v < (int)versions.size(); ++v) {
            int sum = pst.query(versions[v], 0, n-1);
            std::cout << "Version " << v << " sum: " << sum << std::endl;
        }

        std::cout << "\nCan access any previous state without storing full copies!" << std::endl;
    }
};

// Example usage
int main() {
    PersistentDPSegmentTree::dp_with_time_travel();
    PersistentDPSegmentTree::persistent_range_sums();
    PersistentDPSegmentTree::undo_functionality();

    return 0;
}

