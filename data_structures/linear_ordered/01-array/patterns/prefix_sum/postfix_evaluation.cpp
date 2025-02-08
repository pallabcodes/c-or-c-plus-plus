#include <iostream>
// #include <sstream>
#include <stack>
#include <vector>

using namespace std;

int evaluatePostfix(vector<string> &tokens) {
  stack<int> st;

  for (string token : tokens) {
    if (isdigit(token[0]) || (token.size() > 1 && isdigit(token[1]))) {
      st.push(stoi(token));
    } else {
      int b = st.top();
      st.pop();
      int a = st.top();
      st.pop();
      if (token == "+")
        st.push(a + b);
      if (token == "-")
        st.push(a - b);
      if (token == "*")
        st.push(a * b);
      if (token == "/")
        st.push(a / b);
    }
  }
  return st.top();
}

int main() {
  vector<string> postfix = {"3", "4", "5", "*", "+"};
  cout << "Postfix Evaluation: " << evaluatePostfix(postfix)
       << endl; // Output: 23
  return 0;
}
