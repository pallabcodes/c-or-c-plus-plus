#include <iostream>
#include <queue>
#include <unordered_map>
using namespace std;

void firstNonRepeating(string s) {
  unordered_map<char, int> count;
  queue<char> q;

  for (char c : s) {
    count[c]++;
    q.push(c);

    while (!q.empty() && count[q.front()] > 1)
      q.pop();

    cout << (q.empty() ? '#' : q.front()) << " ";
  }
}

int main() {
  firstNonRepeating("aabc");
  // Output: a a # b
  return 0;
}
