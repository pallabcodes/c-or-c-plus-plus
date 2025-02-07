#include <iostream>
#include <stack>
#include <unordered_map>

using namespace std;

bool isValid(string s) {
  stack<char> st;
  unordered_map<char, char> pairs = {{')', '('}, {']', '['}, {'}', '{'}};

  for (char c : s) {
    if (pairs.count(c)) {
      if (st.empty() || st.top() != pairs[c])
        return false;
      st.pop();
    } else {
      st.push(c);
    }
  }
  return st.empty();
}

int main() {
  string s = "{[()]}";
  cout << (isValid(s) ? "Valid" : "Invalid"); // t = O(n), s = O(1)
  return 0;
}
