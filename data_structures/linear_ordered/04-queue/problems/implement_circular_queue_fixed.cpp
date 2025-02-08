#include <iostream>
using namespace std;

class CircularQueue {
  int *arr, front, rear, size;

public:
  CircularQueue(int s) : size(s) {
    arr = new int[s];
    front = rear = -1;
  }

  void enqueue(int x) {
    if ((rear + 1) % size == front)
      cout << "Queue is full\n";
    else {
      if (front == -1)
        front = 0;
      rear = (rear + 1) % size;
      arr[rear] = x;
    }
  }

  void dequeue() {
    if (front == -1)
      cout << "Queue is empty\n";
    else {
      if (front == rear)
        front = rear = -1;
      else
        front = (front + 1) % size;
    }
  }

  int getFront() { return (front == -1) ? -1 : arr[front]; }

  bool isEmpty() { return front == -1; }
};

int main() {
  CircularQueue q(5);
  q.enqueue(10);
  q.enqueue(20);
  q.enqueue(30);
  q.dequeue();
  cout << q.getFront(); // Output: 20
}
