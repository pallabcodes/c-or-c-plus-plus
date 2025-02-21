#include <bits/stdc++.h>
using namespace std;

// The number whose square root we need to find.
// precision → Determines how accurate the result should be (1e-6 means up
// to 6 decimal places).
double findSquareRoot(int x, double precision = 1e-6) {
  if (x < 0) {
    throw invalid_argument(
        "Square root of a negative number is not defined for real numbers.");
  }
  // Square root is itself(√0 = 0, √1 = 1)
  if (x == 0 || x == 1) {
    return x; // Handle 0 and 1 directly
  }

  double left = 0, right = x, mid;

  while (right - left > precision) {
    mid = left + (right - left) / 2;

    // decreasing the search range as needed
    if (mid * mid < x) {
      // mid is too small, so the square root must be greater than mid
      left = mid;
    } else {
      // This means our mid is too large, so the square root must be smaller
      // than mid
      right = mid;
    }
  }

  return (left + right) / 2; // Return the midpoint for better accuracy
}

int main() {
  int x = 10;
  cout << fixed << setprecision(6) << "Square root of " << x
       << " is: " << findSquareRoot(x) << endl;

  return 0;
}
