// #include <iostream>

// using namespace std;

// struct Node {
//   int data;
//   Node *xorPtr; // XOR of prev and next
// };

// Node *XOR(Node *a, Node *b) {
//   return (Node *)((uintptr_t)(a) ^ (uintptr_t)(b));
// }

// void insert(Node *&head, int val) {
//   Node *newNode = new Node{val, head};
//   if (head)
//     head->xorPtr = XOR(newNode, head->xorPtr);
//   head = newNode;
// }

// void traverse(Node *head) {
//   Node *curr = head, *prev = nullptr, *next;
//   while (curr) {
//     cout << curr->data << " ";
//     next = XOR(prev, curr->xorPtr);
//     prev = curr;
//     curr = next;
//   }
// }
