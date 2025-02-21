#include <iostream>
#include <vector>
using namespace std;

void bubbleSort(vector<int> &arr) {
  int n = arr.size();
  bool swapped;

  for (int i = 0; i < n - 1; i++) {
    swapped = false;

    // Last i elements are already sorted
    for (int j = 0; j < n - 1 - i; j++) {
      if (arr[j] > arr[j + 1]) {
        swap(arr[j], arr[j + 1]);
        swapped = true;
      }
    }

    // During any inner loop, if swapped couldn't become true that means it has
    // alread sorted which is why after exiting inner loop, just break main loop
    if (!swapped)
      break;
  }
}

int main() {
  vector<int> arr = {5, 3, 8, 4, 2, 7, 1, 9};

  bubbleSort(arr);

  cout << "Sorted Array: ";

  for (int num : arr) {
    cout << num << " ";
  }

  cout << endl;

  return 0;
}

// are the abve 10 enough or do I need to know any additional techniques to
// tackle any unknown probmlems ?
