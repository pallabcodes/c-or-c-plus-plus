#include <deque>
#include <iostream>
using namespace std;

int main() {
  deque<int> dq;
  dq.push_back(10);
  dq.push_front(20);
  cout << dq.front() << " " << dq.back(); // Output: 20 10
}
