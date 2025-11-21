#include <iostream>
#include <climits>

using namespace std;

// Fibonacci Heap - Advanced heap with O(1) amortized insert/decrease key
// O(log n) extract min
struct FibonacciNode {
    int key;
    int degree;
    bool marked;
    FibonacciNode* parent;
    FibonacciNode* child;
    FibonacciNode* left;
    FibonacciNode* right;

    FibonacciNode(int k) : key(k), degree(0), marked(false),
                           parent(nullptr), child(nullptr) {
        left = right = this;
    }
};

class FibonacciHeap {
private:
    FibonacciNode* minNode;
    int size;

    void link(FibonacciNode* y, FibonacciNode* x) {
        y->left->right = y->right;
        y->right->left = y->left;

        y->parent = x;
        if (!x->child) {
            x->child = y;
            y->left = y->right = y;
        } else {
            y->left = x->child;
            y->right = x->child->right;
            x->child->right = y;
            y->right->left = y;
        }

        x->degree++;
        y->marked = false;
    }

    void consolidate() {
        int maxDegree = static_cast<int>(log2(size)) + 1;
        FibonacciNode* degreeArray[maxDegree + 1];
        for (int i = 0; i <= maxDegree; i++) {
            degreeArray[i] = nullptr;
        }

        FibonacciNode* current = minNode;
        vector<FibonacciNode*> roots;

        do {
            roots.push_back(current);
            current = current->right;
        } while (current != minNode);

        for (FibonacciNode* root : roots) {
            FibonacciNode* x = root;
            int d = x->degree;

            while (degreeArray[d] != nullptr) {
                FibonacciNode* y = degreeArray[d];
                if (x->key > y->key) {
                    swap(x, y);
                }

                if (y == minNode) {
                    minNode = x;
                }

                link(y, x);
                degreeArray[d] = nullptr;
                d++;
            }
            degreeArray[d] = x;
        }

        minNode = nullptr;
        for (int i = 0; i <= maxDegree; i++) {
            if (degreeArray[i] != nullptr) {
                if (!minNode) {
                    minNode = degreeArray[i];
                    minNode->left = minNode->right = minNode;
                } else {
                    degreeArray[i]->left = minNode;
                    degreeArray[i]->right = minNode->right;
                    minNode->right = degreeArray[i];
                    degreeArray[i]->right->left = degreeArray[i];

                    if (degreeArray[i]->key < minNode->key) {
                        minNode = degreeArray[i];
                    }
                }
            }
        }
    }

    void cut(FibonacciNode* x, FibonacciNode* y) {
        if (x->right == x) {
            y->child = nullptr;
        } else {
            x->left->right = x->right;
            x->right->left = x->left;
            if (y->child == x) {
                y->child = x->right;
            }
        }

        y->degree--;

        x->left = minNode;
        x->right = minNode->right;
        minNode->right = x;
        x->right->left = x;

        x->parent = nullptr;
        x->marked = false;
    }

    void cascadingCut(FibonacciNode* y) {
        FibonacciNode* z = y->parent;
        if (z) {
            if (!y->marked) {
                y->marked = true;
            } else {
                cut(y, z);
                cascadingCut(z);
            }
        }
    }

public:
    FibonacciHeap() : minNode(nullptr), size(0) {}

    void insert(int key) {
        FibonacciNode* newNode = new FibonacciNode(key);

        if (!minNode) {
            minNode = newNode;
        } else {
            newNode->left = minNode;
            newNode->right = minNode->right;
            minNode->right = newNode;
            newNode->right->left = newNode;

            if (key < minNode->key) {
                minNode = newNode;
            }
        }

        size++;
    }

    int extractMin() {
        if (!minNode) {
            return -1;
        }

        FibonacciNode* z = minNode;
        FibonacciNode* x = z->child;

        if (x) {
            FibonacciNode* temp = x;
            do {
                FibonacciNode* next = x->right;
                x->left = minNode;
                x->right = minNode->right;
                minNode->right = x;
                x->right->left = x;
                x->parent = nullptr;
                x = next;
            } while (x != temp);
        }

        z->left->right = z->right;
        z->right->left = z->left;

        int minKey = z->key;

        if (z == z->right) {
            minNode = nullptr;
        } else {
            minNode = z->right;
            consolidate();
        }

        size--;
        delete z;
        return minKey;
    }

    int getMin() {
        return minNode ? minNode->key : -1;
    }

    bool isEmpty() {
        return minNode == nullptr;
    }

    void decreaseKey(FibonacciNode* x, int newKey) {
        if (newKey > x->key) {
            return;
        }

        x->key = newKey;
        FibonacciNode* y = x->parent;

        if (y && x->key < y->key) {
            cut(x, y);
            cascadingCut(y);
        }

        if (x->key < minNode->key) {
            minNode = x;
        }
    }
};

int main() {
    FibonacciHeap heap;

    heap.insert(10);
    heap.insert(5);
    heap.insert(20);
    heap.insert(3);
    heap.insert(15);

    cout << "Min: " << heap.getMin() << endl;

    cout << "Extracting: ";
    while (!heap.isEmpty()) {
        cout << heap.extractMin() << " ";
    }
    cout << endl;

    return 0;
}

