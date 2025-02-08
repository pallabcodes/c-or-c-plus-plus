double findSquareRoot(int x, double precision = 1e-6) {
  double left = 0, right = x, mid;
  while (right - left > precision) {
    mid = left + (right - left) / 2;
    if (mid * mid < x)
      left = mid;
    else
      right = mid;
  }
  return left;
}
