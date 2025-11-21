#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

// Compressed Trie (Radix Tree) - Compresses single child nodes
// More memory efficient than standard trie
class CompressedTrie {
private:
    struct Node {
        string label;
        bool isEnd;
        unordered_map<char, Node*> children;

        Node(const string& lbl = "") : label(lbl), isEnd(false) {}
    };

    Node* root;

    void splitNode(Node* node, int pos) {
        string remainingLabel = node->label.substr(pos);
        node->label = node->label.substr(0, pos);

        Node* newChild = new Node(remainingLabel);
        newChild->isEnd = node->isEnd;
        newChild->children = node->children;

        node->isEnd = false;
        node->children.clear();
        node->children[remainingLabel[0]] = newChild;
    }

    bool insert(Node* node, const string& word, int wordPos) {
        if (wordPos == word.length()) {
            bool wasEnd = node->isEnd;
            node->isEnd = true;
            return !wasEnd;
        }

        char ch = word[wordPos];
        if (node->children.find(ch) == node->children.end()) {
            node->children[ch] = new Node(word.substr(wordPos));
            node->children[ch]->isEnd = true;
            return true;
        }

        Node* child = node->children[ch];
        string label = child->label;
        int i = 0;

        while (i < label.length() && wordPos + i < word.length() && 
               label[i] == word[wordPos + i]) {
            i++;
        }

        if (i == label.length()) {
            return insert(child, word, wordPos + i);
        }

        if (i == 0) {
            return false;
        }

        splitNode(child, i);
        return insert(child, word, wordPos + i);
    }

    bool search(Node* node, const string& word, int wordPos) {
        if (wordPos == word.length()) {
            return node->isEnd;
        }

        char ch = word[wordPos];
        if (node->children.find(ch) == node->children.end()) {
            return false;
        }

        Node* child = node->children[ch];
        string label = child->label;

        if (word.length() - wordPos < label.length()) {
            return false;
        }

        for (int i = 0; i < label.length(); i++) {
            if (word[wordPos + i] != label[i]) {
                return false;
            }
        }

        return search(child, word, wordPos + label.length());
    }

    void print(Node* node, string prefix) {
        if (node->isEnd) {
            cout << prefix << node->label << endl;
        }

        for (auto& pair : node->children) {
            print(pair.second, prefix + node->label);
        }
    }

public:
    CompressedTrie() {
        root = new Node();
    }

    bool insert(const string& word) {
        return insert(root, word, 0);
    }

    bool search(const string& word) {
        return search(root, word, 0);
    }

    void printAll() {
        print(root, "");
    }
};

int main() {
    CompressedTrie trie;

    trie.insert("hello");
    trie.insert("hell");
    trie.insert("help");
    trie.insert("helmet");

    cout << "Search 'hello': " << trie.search("hello") << endl;
    cout << "Search 'help': " << trie.search("help") << endl;
    cout << "Search 'hel': " << trie.search("hel") << endl;

    cout << "\nAll words:" << endl;
    trie.printAll();

    return 0;
}

