// Heavy-Light Decomposition: Decompose tree into chains for efficient queries
// Used for path queries, subtree queries, and LCA
// Time: O(log^2 n) per query with segment tree
// Space: O(n)

#include <vector>
#include <iostream>
#include <algorithm>

class HeavyLightDecomposition {
private:
    std::vector<std::vector<int>> tree;
    std::vector<int> parent;
    std::vector<int> depth;
    std::vector<int> size;
    std::vector<int> heavy;
    std::vector<int> head;
    std::vector<int> pos;
    int curPos;
    
    int dfs(int u, int p) {
        parent[u] = p;
        size[u] = 1;
        int maxSize = 0;
        
        for (int v : tree[u]) {
            if (v == p) continue;
            depth[v] = depth[u] + 1;
            int childSize = dfs(v, u);
            size[u] += childSize;
            
            if (childSize > maxSize) {
                maxSize = childSize;
                heavy[u] = v;
            }
        }
        
        return size[u];
    }
    
    void decompose(int u, int h) {
        head[u] = h;
        pos[u] = curPos++;
        
        if (heavy[u] != -1) {
            decompose(heavy[u], h);
        }
        
        for (int v : tree[u]) {
            if (v == parent[u] || v == heavy[u]) continue;
            decompose(v, v);
        }
    }
    
public:
    HeavyLightDecomposition(const std::vector<std::vector<int>>& adjList, int root = 0)
        : tree(adjList) {
        int n = tree.size();
        parent.assign(n, -1);
        depth.assign(n, 0);
        size.assign(n, 0);
        heavy.assign(n, -1);
        head.assign(n, -1);
        pos.assign(n, -1);
        curPos = 0;
        
        dfs(root, -1);
        decompose(root, root);
    }
    
    // Query path from u to v (example: sum/max/min)
    int queryPath(int u, int v) {
        int result = 0;
        
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) {
                std::swap(u, v);
            }
            
            // Query segment [pos[head[u]], pos[u]]
            // result = combine(result, segmentTree.query(pos[head[u]], pos[u]));
            u = parent[head[u]];
        }
        
        if (depth[u] > depth[v]) {
            std::swap(u, v);
        }
        
        // Query segment [pos[u], pos[v]]
        // result = combine(result, segmentTree.query(pos[u], pos[v]));
        
        return result;
    }
    
    // Query subtree rooted at u
    int querySubtree(int u) {
        // Query segment [pos[u], pos[u] + size[u] - 1]
        // return segmentTree.query(pos[u], pos[u] + size[u] - 1);
        return 0;
    }
    
    // Find LCA using HLD
    int lca(int u, int v) {
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) {
                std::swap(u, v);
            }
            u = parent[head[u]];
        }
        return (depth[u] < depth[v]) ? u : v;
    }
    
    std::vector<int> getPositions() const {
        return pos;
    }
    
    std::vector<int> getHeads() const {
        return head;
    }
};

// Example usage
int main() {
    int n = 7;
    std::vector<std::vector<int>> tree(n);
    
    // Example tree
    tree[0].push_back(1);
    tree[0].push_back(2);
    tree[1].push_back(3);
    tree[1].push_back(4);
    tree[2].push_back(5);
    tree[2].push_back(6);
    
    HeavyLightDecomposition hld(tree, 0);
    
    std::cout << "Heavy-Light Decomposition:" << std::endl;
    std::vector<int> pos = hld.getPositions();
    std::vector<int> head = hld.getHeads();
    
    for (int i = 0; i < n; i++) {
        std::cout << "Node " << i << ": pos=" << pos[i] 
                  << ", head=" << head[i] << std::endl;
    }
    
    std::cout << "\nLCA(3, 4) = " << hld.lca(3, 4) << std::endl;
    std::cout << "LCA(3, 5) = " << hld.lca(3, 5) << std::endl;
    std::cout << "LCA(4, 6) = " << hld.lca(4, 6) << std::endl;
    
    return 0;
}

