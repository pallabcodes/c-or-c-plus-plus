#include <iostream>
using namespace std;

// TrieNode structure
struct TrieNode {
  TrieNode *children[26]; // Array of child nodes
  bool isEndOfWord;       // Marks the end of a word
  int prefixCount;        // Counts words sharing this prefix

  TrieNode() {
    isEndOfWord = false;
    prefixCount = 0;
    for (int i = 0; i < 26; i++) {
      children[i] = nullptr;
    }
  }
};

// Trie implementation
class Trie {
private:
  TrieNode *root;

  bool deleteHelper(TrieNode *node, string word, int depth) {
    if (!node) {
      return false;
    }

    if (depth == word.size()) {
      if (!node->isEndOfWord) {
        return false;
      }

      node->isEndOfWord = false;

      // If node has no children, we can delete it
      for (int i = 0; i < 26; i++) {
        if (node->children[i]) {
          return false;
        }
      }
      return true;
    }

    int index = word[depth] - 'a'; // word[depth]  = 'c' - 'a' = 99 - 97 = 2

    if (deleteHelper(node->children[index], word, depth + 1)) {
      delete node->children[index];
      node->children[index] = nullptr;

      // If node has no children and is not end of another word, delete it
      for (int i = 0; i < 26; i++) {
        if (node->children[i]) {
          return false;
        }
      }

      return !node->isEndOfWord;
    }

    return false;
  }

public:
  Trie() { root = new TrieNode(); }

  void insert(string word) {
    TrieNode *node = root;

    for (char c : word) {
      int index =
          c - 'a'; // coverts character to ASCII so e.g. 'a' = 97 so if the c is
                   // 'a' then index = 97 - 97 = 0, similary for c = 99 - 97 = 2
      if (!node->children[index]) {
        node->children[index] = new TrieNode();
      }

      node = node->children[index]; // Move to the child node
      node->prefixCount++;
    }

    node->isEndOfWord = true;
  }

  bool search(string word) {
    TrieNode *node = root;
    for (char c : word) {
      int index = c - 'a';
      if (!node->children[index])
        return false;
      node = node->children[index];
    }
    return node->isEndOfWord;
  }

  bool startsWith(string prefix) {
    TrieNode *node = root;
    for (char c : prefix) {
      int index = c - 'a';
      if (!node->children[index])
        return false;
      node = node->children[index];
    }
    return true;
  }

  int countWordsWithPrefix(string prefix) {
    TrieNode *node = root;
    for (char c : prefix) {
      int index = c - 'a';
      if (!node->children[index])
        return 0;
      node = node->children[index];
    }
    return node->prefixCount;
  }

  void deleteWord(string word) { deleteHelper(root, word, 0); }
};

int main() {
  Trie trie;
  trie.insert("cat");
  trie.insert("cap");
  trie.insert("bat");
  trie.insert("bad");

  cout << trie.search("cap") << endl;  // Output: 1 (true)
  cout << trie.search("bat") << endl;  // Output: 1 (true)
  cout << trie.search("ball") << endl; // Output: 0 (false)

  cout << trie.startsWith("ca") << endl;           // Output: 1 (true)
  cout << trie.countWordsWithPrefix("ba") << endl; // Output: 2 ("bat", "bad")

  trie.deleteWord("bat");
  cout << trie.search("bat") << endl; // Output: 0 (false)
}
