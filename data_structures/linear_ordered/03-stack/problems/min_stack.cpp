`#include<stack>

    using namespace std;

class MinStack {
  stack<int> mainStack, minStack;

public:
  void push(int x) {
    mainStack.push(x);
    if (minStack.empty() || x <= minStack.top())
      minStack.push(x);
  }

  void pop() {
    if (mainStack.top() == minStack.top())
      minStack.pop();
    mainStack.pop();
  }

  int getMin() { return minStack.top(); }
};

int main() { return 0; }