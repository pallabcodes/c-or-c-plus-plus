#include <iostream>
#include <stack>
#include <vector>

using namespace std;

void stockSpan(int prices[], int n) {
  stack<int> s;
  vector<int> span(n);

  for (int i = 0; i < n; i++) {
    while (!s.empty() && prices[s.top()] <= prices[i])
      s.pop();
    span[i] = s.empty() ? (i + 1) : (i - s.top());
    s.push(i);
  }

  for (int x : span)
    cout << x << " ";
}

int main() {
  int prices[] = {100, 80, 60, 70, 60, 75, 85};
  int n = 7;
  stockSpan(prices, n);
  return 0;
}
