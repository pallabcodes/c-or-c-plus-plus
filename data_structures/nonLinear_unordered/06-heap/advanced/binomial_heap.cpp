#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// Binomial Heap - Collection of binomial trees
// O(log n) merge, O(1) amortized insert, O(log n) extract min
struct BinomialNode {
    int key;
    int degree;
    BinomialNode* parent;
    BinomialNode* child;
    BinomialNode* sibling;

    BinomialNode(int k) : key(k), degree(0), parent(nullptr), 
                          child(nullptr), sibling(nullptr) {}
};

class BinomialHeap {
private:
    BinomialNode* head;

    BinomialNode* mergeTrees(BinomialNode* tree1, BinomialNode* tree2) {
        if (tree1->key > tree2->key) {
            swap(tree1, tree2);
        }

        tree2->parent = tree1;
        tree2->sibling = tree1->child;
        tree1->child = tree2;
        tree1->degree++;

        return tree1;
    }

    BinomialNode* mergeHeaps(BinomialNode* heap1, BinomialNode* heap2) {
        BinomialNode* result = nullptr;
        BinomialNode* tail = nullptr;
        BinomialNode* carry = nullptr;

        while (heap1 || heap2 || carry) {
            BinomialNode* node1 = heap1;
            BinomialNode* node2 = heap2;
            BinomialNode* node3 = carry;

            if (heap1) heap1 = heap1->sibling;
            if (heap2) heap2 = heap2->sibling;

            int degree1 = node1 ? node1->degree : -1;
            int degree2 = node2 ? node2->degree : -1;
            int degree3 = node3 ? node3->degree : -1;

            BinomialNode* chosen = nullptr;
            int minDegree = min({degree1 >= 0 ? degree1 : INT_MAX,
                                degree2 >= 0 ? degree2 : INT_MAX,
                                degree3 >= 0 ? degree3 : INT_MAX});

            if (degree1 == minDegree) {
                chosen = node1;
                node1 = nullptr;
            } else if (degree2 == minDegree) {
                chosen = node2;
                node2 = nullptr;
            } else {
                chosen = node3;
                node3 = nullptr;
            }

            if (!result) {
                result = tail = chosen;
            } else {
                tail->sibling = chosen;
                tail = chosen;
            }

            if (node1 && node2 && node1->degree == node2->degree) {
                carry = mergeTrees(node1, node2);
            } else if (node1 && node3 && node1->degree == node3->degree) {
                carry = mergeTrees(node1, node3);
            } else if (node2 && node3 && node2->degree == node3->degree) {
                carry = mergeTrees(node2, node3);
            } else {
                carry = nullptr;
                if (node1) {
                    if (!result) {
                        result = tail = node1;
                    } else {
                        tail->sibling = node1;
                        tail = node1;
                    }
                }
                if (node2) {
                    if (!result) {
                        result = tail = node2;
                    } else {
                        tail->sibling = node2;
                        tail = node2;
                    }
                }
                if (node3) {
                    if (!result) {
                        result = tail = node3;
                    } else {
                        tail->sibling = node3;
                        tail = node3;
                    }
                }
            }
        }

        return result;
    }

    BinomialNode* findMin() {
        if (!head) {
            return nullptr;
        }

        BinomialNode* minNode = head;
        BinomialNode* current = head->sibling;

        while (current) {
            if (current->key < minNode->key) {
                minNode = current;
            }
            current = current->sibling;
        }

        return minNode;
    }

    void reverseList(BinomialNode* node) {
        BinomialNode* prev = nullptr;
        while (node) {
            BinomialNode* next = node->sibling;
            node->sibling = prev;
            node->parent = nullptr;
            prev = node;
            node = next;
        }
    }

public:
    BinomialHeap() : head(nullptr) {}

    void insert(int key) {
        BinomialNode* newNode = new BinomialNode(key);
        head = mergeHeaps(head, newNode);
    }

    int extractMin() {
        if (!head) {
            return -1;
        }

        BinomialNode* minNode = findMin();
        BinomialNode* prev = nullptr;
        BinomialNode* current = head;

        while (current != minNode) {
            prev = current;
            current = current->sibling;
        }

        if (prev) {
            prev->sibling = current->sibling;
        } else {
            head = current->sibling;
        }

        BinomialNode* childList = minNode->child;
        reverseList(childList);

        head = mergeHeaps(head, childList);

        int minKey = minNode->key;
        delete minNode;
        return minKey;
    }

    int getMin() {
        BinomialNode* minNode = findMin();
        return minNode ? minNode->key : -1;
    }

    bool isEmpty() {
        return head == nullptr;
    }

    void merge(BinomialHeap& other) {
        head = mergeHeaps(head, other.head);
        other.head = nullptr;
    }
};

int main() {
    BinomialHeap heap;

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

