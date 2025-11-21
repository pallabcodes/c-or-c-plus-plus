// Link-Cut Tree: Dynamic tree data structure
// Based on research by Sleator and Tarjan
// Supports link, cut, and path queries in O(log n) amortized
// Space: O(n)
// God modded implementation for dynamic connectivity

#include <vector>
#include <iostream>
#include <memory>

struct Node {
    int value;
    int sum;
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
    std::shared_ptr<Node> parent;
    bool reversed;
    
    Node(int v) : value(v), sum(v), left(nullptr), right(nullptr), 
                  parent(nullptr), reversed(false) {}
};

class LinkCutTree {
private:
    std::vector<std::shared_ptr<Node>> nodes;
    
    void update(std::shared_ptr<Node> x) {
        if (!x) return;
        x->sum = x->value;
        if (x->left) x->sum += x->left->sum;
        if (x->right) x->sum += x->right->sum;
    }
    
    void push(std::shared_ptr<Node> x) {
        if (x && x->reversed) {
            x->reversed = false;
            std::swap(x->left, x->right);
            if (x->left) x->left->reversed = !x->left->reversed;
            if (x->right) x->right->reversed = !x->right->reversed;
        }
    }
    
    bool isRoot(std::shared_ptr<Node> x) {
        return !x->parent || 
               (x->parent->left != x && x->parent->right != x);
    }
    
    void rotate(std::shared_ptr<Node> x) {
        std::shared_ptr<Node> p = x->parent;
        std::shared_ptr<Node> g = p->parent;
        
        if (!isRoot(p)) {
            if (g->left == p) g->left = x;
            else g->right = x;
        }
        x->parent = g;
        
        if (p->left == x) {
            p->left = x->right;
            if (x->right) x->right->parent = p;
            x->right = p;
        } else {
            p->right = x->left;
            if (x->left) x->left->parent = p;
            x->left = p;
        }
        
        p->parent = x;
        update(p);
        update(x);
    }
    
    void splay(std::shared_ptr<Node> x) {
        while (!isRoot(x)) {
            std::shared_ptr<Node> p = x->parent;
            std::shared_ptr<Node> g = p->parent;
            
            if (!isRoot(p)) {
                push(g);
            }
            push(p);
            push(x);
            
            if (!isRoot(p)) {
                if ((p->left == x) == (g->left == p)) {
                    rotate(p);
                } else {
                    rotate(x);
                }
            }
            rotate(x);
        }
        push(x);
        update(x);
    }
    
    void access(std::shared_ptr<Node> x) {
        std::shared_ptr<Node> last = nullptr;
        std::shared_ptr<Node> curr = x;
        
        while (curr) {
            splay(curr);
            curr->right = last;
            update(curr);
            last = curr;
            curr = curr->parent;
        }
        splay(x);
    }
    
    void makeRoot(std::shared_ptr<Node> x) {
        access(x);
        x->reversed = !x->reversed;
        push(x);
    }
    
    std::shared_ptr<Node> findRoot(std::shared_ptr<Node> x) {
        access(x);
        while (x->left) {
            push(x);
            x = x->left;
        }
        splay(x);
        return x;
    }
    
public:
    LinkCutTree(int n) {
        nodes.resize(n);
        for (int i = 0; i < n; i++) {
            nodes[i] = std::make_shared<Node>(0);
        }
    }
    
    void link(int u, int v) {
        makeRoot(nodes[u]);
        nodes[u]->parent = nodes[v];
    }
    
    void cut(int u, int v) {
        makeRoot(nodes[u]);
        access(nodes[v]);
        nodes[v]->left = nullptr;
        nodes[u]->parent = nullptr;
        update(nodes[v]);
    }
    
    bool connected(int u, int v) {
        return findRoot(nodes[u]) == findRoot(nodes[v]);
    }
    
    void updateValue(int u, int value) {
        access(nodes[u]);
        nodes[u]->value = value;
        update(nodes[u]);
    }
    
    int pathSum(int u, int v) {
        makeRoot(nodes[u]);
        access(nodes[v]);
        return nodes[v]->sum;
    }
};

// Example usage
int main() {
    LinkCutTree lct(5);
    
    lct.updateValue(0, 1);
    lct.updateValue(1, 2);
    lct.updateValue(2, 3);
    lct.updateValue(3, 4);
    lct.updateValue(4, 5);
    
    lct.link(0, 1);
    lct.link(1, 2);
    lct.link(2, 3);
    
    std::cout << "Connected(0, 3): " << (lct.connected(0, 3) ? "Yes" : "No") << std::endl;
    std::cout << "Path sum from 0 to 3: " << lct.pathSum(0, 3) << std::endl;
    
    lct.cut(1, 2);
    std::cout << "After cut, connected(0, 3): " << (lct.connected(0, 3) ? "Yes" : "No") << std::endl;
    
    return 0;
}

