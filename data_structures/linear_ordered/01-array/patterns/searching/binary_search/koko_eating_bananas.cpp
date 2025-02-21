#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
  // Function to calculate total hours needed at a given eating speed
  bool canEatAll(vector<int> &piles, int h, int speed) {
    int hours = 0;

    for (int bananas : piles) {
      hours +=
          (bananas + speed - 1) / speed; // Equivalent to ceil(bananas / speed)
    }

    return hours <= h;
  }

  int minEatingSpeed(vector<int> &piles, int h) {
    int left = 1,
        right = *max_element(piles.begin(), piles.end()); // Search range

    while (left < right) {
      int mid = left + (right - left) / 2; // Avoid overflow

      if (canEatAll(piles, h, mid)) {
        right = mid; // Try lower speed
      } else {
        left = mid + 1; // Increase speed
      }
    }

    return left; // Minimum speed needed
  }
};

// Driver Code
int main() {
  Solution solution;
  vector<int> piles = {3, 6, 7, 11};
  int h = 8;

  cout << "Minimum eating speed: " << solution.minEatingSpeed(piles, h) << endl;
  return 0;
}
