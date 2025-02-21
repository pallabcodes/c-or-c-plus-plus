#include <iostream>
#include <vector>
using namespace std;

// Insertion Sort for sorting individual buckets
void insertionSort(vector<float> &bucket) {
  for (int i = 1; i < bucket.size(); i++) {
    float key = bucket[i];
    int j = i - 1;

    while (j >= 0 && bucket[j] > key) {
      bucket[j + 1] = bucket[j]; // Shift right
      j--;
    }
    bucket[j + 1] = key;
  }
}

void bucketSort(vector<float> &arr) {
  int n = arr.size();
  if (n <= 1)
    return; // Already sorted

  // Create n empty buckets
  vector<vector<float>> buckets(n);

  // 1. Distribute elements into buckets
  for (float num : arr) {
    int index = num * n; // Scaling to fit bucket index
    buckets[index].push_back(num);
  }

  // 2. Sort each bucket using Insertion Sort
  for (auto &bucket : buckets) {
    if (!bucket.empty())
      insertionSort(bucket);
  }

  // 3. Merge buckets back into the original array
  int idx = 0;
  for (auto &bucket : buckets) {
    for (float num : bucket) {
      arr[idx++] = num;
    }
  }
}

int main() {
  vector<float> arr = {0.78, 0.17, 0.39, 0.26, 0.72,
                       0.94, 0.21, 0.12, 0.23, 0.68};

  bucketSort(arr);

  cout << "Sorted Array: ";
  for (float num : arr) {
    cout << num << " ";
  }
  cout << endl;

  return 0;
}
