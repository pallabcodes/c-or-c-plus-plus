#include <bits/stdc++.h>
using namespace std;

class MinHeap {
private:
  vector<int> heap;

  // Heapify down (used for building heap)
  void heapifyDown(int i) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < heap.size() && heap[left] < heap[smallest])
      smallest = left;
    if (right < heap.size() && heap[right] < heap[smallest])
      smallest = right;

    if (smallest != i) {
      swap(heap[i], heap[smallest]);
      heapifyDown(smallest);
    }
  }

public:
  // Build heap from an unordered array (O(n) time complexity)
  void buildHeap(vector<int> &arr) {
    heap = arr; // Copy elements
    for (int i = heap.size() / 2 - 1; i >= 0;
         i--) { // Start from last non-leaf node
      heapifyDown(i);
    }
  }

  void printHeap() {
    for (int val : heap)
      cout << val << " ";
    cout << endl;
  }
};

int main() {
  vector<int> arr = {3, 1, 6, 5, 2, 4};
  MinHeap minHeap;
  minHeap.buildHeap(arr);

  cout << "Heap after heapify: ";
  minHeap.printHeap();
  return 0;
}
