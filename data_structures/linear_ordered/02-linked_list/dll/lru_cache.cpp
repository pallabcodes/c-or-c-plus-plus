#include <unordered_map>

using namespace std;

class LRUCache {
public:
  struct Node {
    int key, value;
    Node *prev, *next;
    Node(int k, int v) : key(k), value(v), prev(nullptr), next(nullptr) {}
  };

  unordered_map<int, Node *> cache;
  int capacity;
  Node *head, *tail;

  LRUCache(int cap) {
    capacity = cap;
    head = new Node(0, 0);
    tail = new Node(0, 0);
    head->next = tail;
    tail->prev = head;
  }

  void moveToHead(Node *node) {
    remove(node);
    insertToHead(node);
  }

  void insertToHead(Node *node) {
    node->next = head->next;
    head->next->prev = node;
    head->next = node;
    node->prev = head;
  }

  void remove(Node *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
  }

  int get(int key) {
    if (cache.find(key) == cache.end())
      return -1;
    moveToHead(cache[key]);
    return cache[key]->value;
  }

  void put(int key, int value) {
    if (cache.find(key) != cache.end()) {
      cache[key]->value = value;
      moveToHead(cache[key]);
      return;
    }

    if (cache.size() == capacity) {
      cache.erase(tail->prev->key);
      remove(tail->prev);
    }

    Node *newNode = new Node(key, value);
    cache[key] = newNode;
    insertToHead(newNode);
  }
};
