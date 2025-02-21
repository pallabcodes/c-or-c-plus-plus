#include <bits/stdc++.h>
using namespace std;

double findSquareRoot(int x, double precision = 1e-6) {
  if (x < 0) {
    throw invalid_argument(
        "Square root of a negative number is not defined for real numbers.");
  }
  if (x == 0 || x == 1)
    return x; // Handle 0 and 1 directly

  double left = 0, right = x, mid;

  while (right - left > precision) {
    mid = left + (right - left) / 2;
    if (mid * mid < x)
      left = mid;
    else
      right = mid;
  }

  return (left + right) / 2; // Return the midpoint for better accuracy
}

int main() {
  int x = 10;
  cout << fixed << setprecision(6) << "Square root of " << x
       << " is: " << findSquareRoot(x) << endl;

  return 0;
}
