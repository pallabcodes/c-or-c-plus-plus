#include <iostream>
#include <vector>

using namespace std;

// Circular Buffer Implementation
template <typename T> class CircularBuffer {
private:
  vector<T> buffer;
  size_t head = 0, tail = 0, size = 0, capacity;

public:
  CircularBuffer(size_t cap) : capacity(cap), buffer(cap) {}

  void push(T item) {
    if (isFull()) {
      cout << "Buffer full, overwriting: " << buffer[head] << endl;
      head = (head + 1) % capacity;
      size--;
    }
    buffer[tail] = item;
    tail = (tail + 1) % capacity;
    size++;
  }

  T pop() {
    if (isEmpty()) {
      throw runtime_error("Buffer empty");
    }
    T item = buffer[head];
    head = (head + 1) % capacity;
    size--;
    return item;
  }

  T peek() const {
    if (isEmpty()) {
      throw runtime_error("Buffer empty");
    }
    return buffer[head];
  }

  bool isEmpty() const { return size == 0; }
  bool isFull() const { return size == capacity; }
  size_t getSize() const { return size; }
  size_t getCapacity() const { return capacity; }
};

int main() {
  // Create a circular buffer of size 5
  CircularBuffer<int> buffer(5);

  // Insert elements
  buffer.push(10);
  buffer.push(20);
  buffer.push(30);
  buffer.push(40);
  buffer.push(50);

  cout << "Peek front element: " << buffer.peek() << endl; // 10

  // Overwrite behavior
  buffer.push(60);                                           // Overwrites 10
  cout << "Peek after overwrite: " << buffer.peek() << endl; // 20

  // Remove elements
  cout << "Popped: " << buffer.pop() << endl; // 20
  cout << "Popped: " << buffer.pop() << endl; // 30

  // Insert more elements
  buffer.push(70);
  buffer.push(80);

  // Pop remaining elements
  while (!buffer.isEmpty()) {
    cout << "Popped: " << buffer.pop() << endl;
  }

  return 0;
}
