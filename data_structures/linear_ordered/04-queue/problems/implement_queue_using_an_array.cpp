#include <iostream>
using namespace std;

class Queue {
  int arr[1000]; // Fixed size array
  int front, rear;

public:
  Queue() {
    front = 0;
    rear = 0;
  }

  void enqueue(int x) {
    arr[rear++] = x; // Insert at rear
  }

  void dequeue() {
    if (front == rear)
      cout << "Queue is empty\n";
    else
      front++; // Move front forward
  }

  int getFront() { return (front == rear) ? -1 : arr[front]; }

  bool isEmpty() { return front == rear; }
};

int main() {
  Queue q;
  q.enqueue(5);
  q.enqueue(10);
  q.enqueue(15);
  q.dequeue();
  cout << q.getFront(); // Output: 10
}
