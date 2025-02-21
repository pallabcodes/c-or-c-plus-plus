#include <iostream>
#include <string>
#include <vector>

using namespace std;

int evaluatePostfix(vector<string> &tokens) {
  int j = 0; // Write pointer for storing numbers and results

  for (int i = 0; i < tokens.size(); i++) { // Iterate over each token
    if (isdigit(tokens[i][0]) ||
        (tokens[i].size() > 1 && isdigit(tokens[i][1]))) {
      // If the token is a number (handles negative numbers too)
      tokens[j++] =
          tokens[i]; // Store the number at index j, then move j forward
    } else {
      // If the token is an operator (+, -, *, /)
      int b =
          stoi(tokens[--j]); // Convert last stored number to int, move j back
      int a = stoi(
          tokens[--j]); // Convert second-last stored number to int, move j back

      // Perform the operation
      if (tokens[i] == "+") {
        a += b;
      }
      if (tokens[i] == "-") {
        a -= b;
      }
      if (tokens[i] == "*") {
        a *= b;
      }
      if (tokens[i] == "/") {
        a /= b;
      }

      tokens[j++] = to_string(a); // Store result at new j position
    }
  }

  // with postfix evaluation, the correct output should be available at oth position which is why below
  return stoi(tokens[0]);
}

int main() {
  vector<string> postfix = {"3", "4", "5", "*", "+"};
  cout << "Postfix Evaluation: " << evaluatePostfix(postfix) << endl;
  return 0;
}