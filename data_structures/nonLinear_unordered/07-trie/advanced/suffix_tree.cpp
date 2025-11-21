#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

// Suffix Tree - Compressed trie of all suffixes
// O(n) construction using Ukkonen's algorithm (simplified version)
class SuffixTree {
private:
    struct Node {
        int start;
        int* end;
        Node* suffixLink;
        unordered_map<char, Node*> children;
        int suffixIndex;

        Node(int s, int* e) : start(s), end(e), suffixLink(nullptr), suffixIndex(-1) {}
    };

    Node* root;
    Node* lastNewNode;
    Node* activeNode;
    int activeEdge;
    int activeLength;
    int remainingSuffixCount;
    int leafEnd;
    vector<int> suffixArray;

    string text;
    int size;

    int edgeLength(Node* node) {
        return *(node->end) - node->start + 1;
    }

    bool walkDown(Node* currentNode) {
        if (activeLength >= edgeLength(currentNode)) {
            activeEdge += edgeLength(currentNode);
            activeLength -= edgeLength(currentNode);
            activeNode = currentNode;
            return true;
        }
        return false;
    }

    void extendSuffixTree(int pos) {
        leafEnd = pos;
        remainingSuffixCount++;
        lastNewNode = nullptr;

        while (remainingSuffixCount > 0) {
            if (activeLength == 0) {
                activeEdge = pos;
            }

            if (activeNode->children.find(text[activeEdge]) == activeNode->children.end()) {
                activeNode->children[text[activeEdge]] = new Node(pos, &leafEnd);
                if (lastNewNode) {
                    lastNewNode->suffixLink = activeNode;
                    lastNewNode = nullptr;
                }
            } else {
                Node* next = activeNode->children[text[activeEdge]];
                if (walkDown(next)) {
                    continue;
                }

                if (text[next->start + activeLength] == text[pos]) {
                    if (lastNewNode && activeNode != root) {
                        lastNewNode->suffixLink = activeNode;
                        lastNewNode = nullptr;
                    }
                    activeLength++;
                    break;
                }

                int* splitEnd = new int;
                *splitEnd = next->start + activeLength - 1;

                Node* split = new Node(next->start, splitEnd);
                activeNode->children[text[activeEdge]] = split;

                split->children[text[pos]] = new Node(pos, &leafEnd);
                next->start += activeLength;
                split->children[text[next->start]] = next;

                if (lastNewNode) {
                    lastNewNode->suffixLink = split;
                }

                lastNewNode = split;
            }

            remainingSuffixCount--;
            if (activeNode == root && activeLength > 0) {
                activeLength--;
                activeEdge = pos - remainingSuffixCount + 1;
            } else if (activeNode != root) {
                activeNode = activeNode->suffixLink;
            }
        }
    }

    void setSuffixIndex(Node* node, int labelHeight) {
        if (!node) return;

        bool leaf = true;
        for (auto& pair : node->children) {
            leaf = false;
            setSuffixIndex(pair.second, labelHeight + edgeLength(pair.second));
        }

        if (leaf) {
            node->suffixIndex = size - labelHeight;
        }
    }

    void buildSuffixArray(Node* node) {
        if (!node) return;

        if (node->suffixIndex != -1) {
            suffixArray.push_back(node->suffixIndex);
            return;
        }

        for (auto& pair : node->children) {
            buildSuffixArray(pair.second);
        }
    }

public:
    SuffixTree(const string& txt) : text(txt + "$"), size(txt.length() + 1) {
        root = new Node(-1, new int(-1));
        activeNode = root;
        activeEdge = -1;
        activeLength = 0;
        remainingSuffixCount = 0;
        leafEnd = -1;

        for (int i = 0; i < size; i++) {
            extendSuffixTree(i);
        }

        setSuffixIndex(root, 0);
        buildSuffixArray(root);
    }

    bool search(const string& pattern) {
        Node* current = root;
        int patternPos = 0;

        while (patternPos < pattern.length()) {
            if (current->children.find(pattern[patternPos]) == current->children.end()) {
                return false;
            }

            Node* child = current->children[pattern[patternPos]];
            string edge = text.substr(child->start, edgeLength(child));

            int i = 0;
            while (i < edge.length() && patternPos < pattern.length() && 
                   edge[i] == pattern[patternPos]) {
                i++;
                patternPos++;
            }

            if (patternPos == pattern.length()) {
                return true;
            }

            if (i < edge.length()) {
                return false;
            }

            current = child;
        }

        return true;
    }

    vector<int> getSuffixArray() {
        return suffixArray;
    }
};

int main() {
    string text = "banana";
    SuffixTree st(text);

    cout << "Search 'ana': " << st.search("ana") << endl;
    cout << "Search 'ban': " << st.search("ban") << endl;
    cout << "Search 'xyz': " << st.search("xyz") << endl;

    vector<int> sa = st.getSuffixArray();
    cout << "Suffix Array: ";
    for (int idx : sa) {
        cout << idx << " ";
    }
    cout << endl;

    return 0;
}

