#include <iostream>

#include <stack>
#include <vector>

using namespace std;

// Previous smaller element (Using Monotonic Increasing Stack)

// Problem: Find the closest smallest element to the left for each element, if
// no smaller element exists, return -1

void previousSmaller(int arr[], int n) {
  stack<int> s;
  vector<int> result(n, -1);

  for (int i = 0; i < n; i++) {
    while (!s.empty() && s.top() >= arr[i])
      s.pop();
    if (!s.empty())
      result[i] = s.top();
    s.push(arr[i]);
  }

  for (int x : result)
    cout << x << " ";
}

int main() {
  int arr[] = {4, 10, 5, 8, 20, 15, 3, 12};
  int n = 8;
  previousSmaller(arr, n);
  return 0;
}
