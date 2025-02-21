#include <iostream>
#include <vector>
using namespace std;

void shellSort(vector<int> &arr) {
  int n = arr.size();

  // Use decreasing gap sequence (Knuth’s sequence could be used too)
  for (int gap = n / 2; gap > 0; gap /= 2) {

    for (int i = gap; i < n; i++) {

      int temp = arr[i], 
          j; // j with no initliazed value so it contains an undefined (garbage)
             // value here where temp assinged with value of arr[i]

      // Insertion sort logic for elements at gap distance

      // Yes, j = i (initialized before the loop), so even if the inner loop
      // doesn't run, j retains its original value (i), ensuring arr[j] = temp;
      // is valid.
      for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) {
        arr[j] = arr[j - gap];
      }

      arr[j] = temp; // {3, 2, 12, 34, 54}
    }
  }
}

// Driver Code
int main() {
  vector<int> arr = {12, 34, 54, 2, 3};
  shellSort(arr);

  cout << "Sorted Array: ";

  for (int num : arr) {
    cout << num << " ";
  }

  cout << endl;
  return 0;
}
