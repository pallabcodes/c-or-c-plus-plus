#include <iostream>
#include <vector>

using namespace std;

vector<int> computePostfixSum(vector<int> &arr) {
  int n = arr.size();
  vector<int> postfix(n);
  postfix[n - 1] = arr[n - 1];

  for (int i = n - 2; i >= 0; i--) {
    postfix[i] = arr[i] + postfix[i + 1];
  }
  return postfix;
}

int main() {
  vector<int> arr = {2, 4, 6, 8, 10};
  vector<int> postfix = computePostfixSum(arr);

  for (int sum : postfix) {
    cout << sum << " ";
  }
  return 0;
}
