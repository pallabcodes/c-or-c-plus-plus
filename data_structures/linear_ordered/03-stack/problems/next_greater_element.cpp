#include <iostream>
#include <stack>
#include <vector>

using namespace std;

void nextGreater(int arr[], int n) {
  // creates an empty stack
  stack<int> s;
  // this vector will have size of n and it will be filled defaultValue of -1
  vector<int> result(n, -1);

  for (int i = 0; i < n; i++) {
    while (!s.empty() && arr[s.top()] < arr[i]) {
      result[s.top()] = arr[i];
      s.pop();
    }

    s.push(i);
  }

  for (int x : result) {
    cout << x << " ";
  }
}

int main() {
  int arr[] = {4, 5, 2, 10, 8};
  int n = 5;
  nextGreater(arr, n); // t = O(n), O(1)
  return 0;
}
