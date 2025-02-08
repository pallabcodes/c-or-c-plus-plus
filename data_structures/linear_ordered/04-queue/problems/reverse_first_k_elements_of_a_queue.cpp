#include <iostream>
#include <queue>
#include <stack>
using namespace std;

void reverseFirstK(queue<int> &q, int k) {
  stack<int> s;
  for (int i = 0; i < k; i++) {
    s.push(q.front());
    q.pop();
  }

  while (!s.empty()) {
    q.push(s.top());
    s.pop();
  }

  int size = q.size() - k;
  while (size--) {
    q.push(q.front());
    q.pop();
  }
}

int main() {
  queue<int> q;
  q.push(10);
  q.push(20);
  q.push(30);
  q.push(40);
  q.push(50);
  reverseFirstK(q, 3);
  while (!q.empty()) {
    cout << q.front() << " "; // Output: 30 20 10 40 50
    q.pop();
  }
}
