// Persistent Segment Tree: Version-controlled segment tree
// Supports queries on previous versions of the tree
// Time: O(log n) per update/query
// Space: O(n log n)
// God modded implementation for time-travel queries

#include <vector>
#include <iostream>
#include <memory>

struct Node {
    long long value;
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
    
    Node(long long v = 0) : value(v), left(nullptr), right(nullptr) {}
    
    Node(std::shared_ptr<Node> l, std::shared_ptr<Node> r) 
        : left(l), right(r) {
        value = 0;
        if (left) value += left->value;
        if (right) value += right->value;
    }
};

class PersistentSegmentTree {
private:
    int n;
    std::vector<std::shared_ptr<Node>> roots;
    
    std::shared_ptr<Node> build(const std::vector<long long>& arr, int left, int right) {
        if (left == right) {
            return std::make_shared<Node>(arr[left]);
        }
        
        int mid = (left + right) / 2;
        auto leftChild = build(arr, left, mid);
        auto rightChild = build(arr, mid + 1, right);
        
        return std::make_shared<Node>(leftChild, rightChild);
    }
    
    std::shared_ptr<Node> update(std::shared_ptr<Node> node, int left, int right, 
                                 int pos, long long value) {
        if (left == right) {
            return std::make_shared<Node>(value);
        }
        
        int mid = (left + right) / 2;
        
        if (pos <= mid) {
            auto newLeft = update(node->left ? node->left : std::make_shared<Node>(), 
                                 left, mid, pos, value);
            return std::make_shared<Node>(newLeft, node->right);
        } else {
            auto newRight = update(node->right ? node->right : std::make_shared<Node>(), 
                                  mid + 1, right, pos, value);
            return std::make_shared<Node>(node->left, newRight);
        }
    }
    
    long long query(std::shared_ptr<Node> node, int left, int right, 
                   int qLeft, int qRight) {
        if (!node || qRight < left || qLeft > right) {
            return 0;
        }
        
        if (qLeft <= left && right <= qRight) {
            return node->value;
        }
        
        int mid = (left + right) / 2;
        return query(node->left, left, mid, qLeft, qRight) +
               query(node->right, mid + 1, right, qLeft, qRight);
    }
    
public:
    PersistentSegmentTree(const std::vector<long long>& arr) {
        n = arr.size();
        roots.push_back(build(arr, 0, n - 1));
    }
    
    int update(int version, int pos, long long value) {
        auto newRoot = update(roots[version], 0, n - 1, pos, value);
        roots.push_back(newRoot);
        return roots.size() - 1;
    }
    
    long long query(int version, int qLeft, int qRight) {
        return query(roots[version], 0, n - 1, qLeft, qRight);
    }
    
    int getLatestVersion() {
        return roots.size() - 1;
    }
};

// Example usage
int main() {
    std::vector<long long> arr = {1, 2, 3, 4, 5};
    
    PersistentSegmentTree pst(arr);
    
    std::cout << "Initial array sum [0, 4]: " << pst.query(0, 0, 4) << std::endl;
    
    int v1 = pst.update(0, 0, 10);
    std::cout << "After updating index 0 to 10, sum [0, 4]: " 
              << pst.query(v1, 0, 4) << std::endl;
    
    int v2 = pst.update(v1, 2, 20);
    std::cout << "After updating index 2 to 20, sum [0, 4]: " 
              << pst.query(v2, 0, 4) << std::endl;
    
    std::cout << "Querying old version [0, 4]: " << pst.query(0, 0, 4) << std::endl;
    std::cout << "Querying version 1 [0, 4]: " << pst.query(v1, 0, 4) << std::endl;
    std::cout << "Querying version 2 [0, 4]: " << pst.query(v2, 0, 4) << std::endl;
    
    return 0;
}
