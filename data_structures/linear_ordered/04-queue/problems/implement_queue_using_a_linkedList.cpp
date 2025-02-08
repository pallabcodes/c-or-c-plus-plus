#include <iostream>
using namespace std;

class Node {
public:
  int data;
  Node *next;
  Node(int x) : data(x), next(nullptr) {}
};

class Queue {
  Node *front, *rear;

public:
  Queue() { front = rear = nullptr; }

  void enqueue(int x) {
    Node *temp = new Node(x);
    if (!rear)
      front = rear = temp;
    else {
      rear->next = temp;
      rear = temp;
    }
  }

  void dequeue() {
    if (!front)
      cout << "Queue is empty\n";
    else {
      Node *temp = front;
      front = front->next;
      if (!front)
        rear = nullptr;
      delete temp;
    }
  }

  int getFront() { return front ? front->data : -1; }

  bool isEmpty() { return front == nullptr; }
};

int main() {
  Queue q;
  q.enqueue(5);
  q.enqueue(10);
  q.enqueue(15);
  q.dequeue();
  cout << q.getFront(); // Output: 10
}
