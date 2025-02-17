#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

// Skip List Implementation
template <typename K, typename V> class SkipList {
private:
  struct Node {
    K key;
    V value;
    vector<Node *> forward;

    Node(K k, V v, int level) : key(k), value(v), forward(level, nullptr) {}
  };

  int max_level;
  float p;
  int current_level;
  Node *head;

  int randomLevel() {
    int level = 1;
    while ((rand() % 100) < (p * 100) && level < max_level)
      level++;
    return level;
  }

public:
  SkipList(int max_lvl = 16, float prob = 0.5)
      : max_level(max_lvl), p(prob), current_level(1) {
    head = new Node(K(), V(), max_level);
  }

  void insert(K key, V value) {
    vector<Node *> update(max_level, nullptr);
    Node *current = head;

    for (int i = max_level - 1; i >= 0; i--) {
      while (current->forward[i] && current->forward[i]->key < key)
        current = current->forward[i];
      update[i] = current;
    }

    int newLevel = randomLevel();
    if (newLevel > current_level) {
      for (int i = current_level; i < newLevel; i++)
        update[i] = head;
      current_level = newLevel;
    }

    Node *newNode = new Node(key, value, newLevel);
    for (int i = 0; i < newLevel; i++) {
      newNode->forward[i] = update[i]->forward[i];
      update[i]->forward[i] = newNode;
    }
  }

  bool search(K key) {
    Node *current = head;

    for (int i = max_level - 1; i >= 0; i--) {
      while (current->forward[i] && current->forward[i]->key < key)
        current = current->forward[i];
    }

    current = current->forward[0];
    return current && current->key == key;
  }

  void erase(K key) {
    vector<Node *> update(max_level, nullptr);
    Node *current = head;

    for (int i = max_level - 1; i >= 0; i--) {
      while (current->forward[i] && current->forward[i]->key < key)
        current = current->forward[i];
      update[i] = current;
    }

    current = current->forward[0];
    if (current && current->key == key) {
      for (int i = 0; i < max_level; i++) {
        if (update[i]->forward[i] != current)
          break;
        update[i]->forward[i] = current->forward[i];
      }
      delete current;
    }
  }

  void display() {
    for (int i = 0; i < current_level; i++) {
      Node *current = head->forward[i];
      cout << "Level " << i + 1 << ": ";
      while (current) {
        cout << "(" << current->key << ", " << current->value << ") ";
        current = current->forward[i];
      }
      cout << endl;
    }
  }
};

int main() {
  SkipList<int, string> skipList;

  skipList.insert(3, "Three");
  skipList.insert(6, "Six");
  skipList.insert(7, "Seven");
  skipList.insert(9, "Nine");
  skipList.insert(12, "Twelve");
  skipList.insert(19, "Nineteen");
  skipList.insert(17, "Seventeen");
  skipList.insert(26, "Twenty-Six");

  cout << "Skip List after insertions:\n";
  skipList.display();

  cout << "\nSearching for 9: " << (skipList.search(9) ? "Found" : "Not Found")
       << endl;
  cout << "Searching for 15: " << (skipList.search(15) ? "Found" : "Not Found")
       << endl;

  skipList.erase(6);
  cout << "\nSkip List after deleting 6:\n";
  skipList.display();

  return 0;
}
