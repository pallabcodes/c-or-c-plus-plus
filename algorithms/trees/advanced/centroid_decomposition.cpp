// Centroid Decomposition: Decompose tree into centroids
// Useful for path queries and tree problems
// Time: O(n log n) for decomposition
// Space: O(n)

#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

class CentroidDecomposition {
private:
    vector<vector<int>> tree;
    vector<bool> removed;
    vector<int> subtreeSize;
    vector<int> parent;
    vector<vector<int>> centroidTree;
    
    void dfsSize(int u, int p) {
        subtreeSize[u] = 1;
        for (int v : tree[u]) {
            if (v != p && !removed[v]) {
                dfsSize(v, u);
                subtreeSize[u] += subtreeSize[v];
            }
        }
    }
    
    int findCentroid(int u, int p, int totalSize) {
        for (int v : tree[u]) {
            if (v != p && !removed[v] && 
                subtreeSize[v] > totalSize / 2) {
                return findCentroid(v, u, totalSize);
            }
        }
        return u;
    }
    
    int decompose(int u) {
        dfsSize(u, -1);
        int centroid = findCentroid(u, -1, subtreeSize[u]);
        removed[centroid] = true;
        
        for (int v : tree[centroid]) {
            if (!removed[v]) {
                int childCentroid = decompose(v);
                centroidTree[centroid].push_back(childCentroid);
                parent[childCentroid] = centroid;
            }
        }
        
        return centroid;
    }
    
public:
    CentroidDecomposition(const vector<vector<int>>& adjList)
        : tree(adjList) {
        int n = tree.size();
        removed.assign(n, false);
        subtreeSize.assign(n, 0);
        parent.assign(n, -1);
        centroidTree.assign(n, vector<int>());
    }
    
    int build() {
        return decompose(0);
    }
    
    vector<vector<int>> getCentroidTree() const {
        return centroidTree;
    }
    
    vector<int> getParents() const {
        return parent;
    }
};

// Example usage
int main() {
    int n = 7;
    vector<vector<int>> tree(n);
    
    tree[0].push_back(1);
    tree[0].push_back(2);
    tree[1].push_back(3);
    tree[1].push_back(4);
    tree[2].push_back(5);
    tree[2].push_back(6);
    
    CentroidDecomposition cd(tree);
    int root = cd.build();
    
    cout << "Centroid Decomposition Root: " << root << endl;
    
    vector<vector<int>> centroidTree = cd.getCentroidTree();
    vector<int> parents = cd.getParents();
    
    cout << "\nCentroid Tree Structure:" << endl;
    for (int i = 0; i < n; i++) {
        if (!centroidTree[i].empty()) {
            cout << "Centroid " << i << " has children: ";
            for (int child : centroidTree[i]) {
                cout << child << " ";
            }
            cout << endl;
        }
    }
    
    return 0;
}

