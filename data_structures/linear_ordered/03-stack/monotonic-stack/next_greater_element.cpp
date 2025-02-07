#include <iostream>

#include <stack>
#include <vector>

using namespace std;

// Next Greater Element (using Monotonic Decreasing stack)

// Problem:  Find the next greater element for each element in the array, if no
// greater element exists, then return -1

void nextGreater(int arr[], int n) {
  stack<int> s;
  vector<int> result(n, -1);

  for (int i = 0; i < n; i++) {
    while (!s.empty() && arr[s.top()] < arr[i]) {
      result[s.top()] = arr[i];
      s.pop();
    }
    s.push(i);
  }

  for (int x : result)
    cout << x << " ";
}

int main() {
  int arr[] = {2, 1, 4, 3};
  int n = 4;
  nextGreater(arr, n);
  return 0;
}
