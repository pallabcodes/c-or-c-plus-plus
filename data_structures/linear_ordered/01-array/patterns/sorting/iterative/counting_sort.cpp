#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

void countingSort(vector<int> &arr) {
  if (arr.empty())
    return;

  // Find the maximum value in the array and store it in this `maxVal` i.e. 8
  int maxVal = *max_element(arr.begin(), arr.end());

  // Create a frequency array and fill it with 0
  vector<int> count(maxVal + 1, 0);

  // Store the frequency of each element
  for (int num : arr)
    count[num]++;

  // N.B: each values is mapped to their indexes so it tells about each element
  // as follows at index 1 has 1 so means 1 has 1 time in the given array

  // count = {0, 1, 2, 2, 1, 0, 0, 0, 1}

  // Reconstruct the sorted array
  int index = 0;

  for (int i = 0; i <= maxVal; i++) {
    while (count[i]-- > 0) {
      arr[index++] = i;
    }
  }
}

// Driver Code
int main() {
  vector<int> arr = {4, 2, 2, 8, 3, 3, 1};
  countingSort(arr);

  cout << "Sorted Array: ";

  for (int num : arr) {
    cout << num << " ";
  }

  cout << endl;
  return 0;
}
