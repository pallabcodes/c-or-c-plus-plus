#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
  // Function to check if a given capacity can ship within D days
  bool canShip(vector<int> &weights, int D, int capacity) {
    int days = 1, currentLoad = 0;

    for (int weight : weights) {
      if (currentLoad + weight > capacity) {
        days++; // Need a new day
        currentLoad = 0;
      }
      currentLoad += weight;
    }

    return days <= D;
  }

  int shipWithinDays(vector<int> &weights, int D) {
    int left = *max_element(weights.begin(), weights.end());   // Min capacity
    int right = accumulate(weights.begin(), weights.end(), 0); // Max capacity

    while (left < right) {
      int mid = left + (right - left) / 2; // Avoid overflow

      if (canShip(weights, D, mid)) {
        right = mid; // Try smaller capacity
      } else {
        left = mid + 1; // Increase capacity
      }
    }

    return left; // Minimum capacity required
  }
};

// Driver Code
int main() {
  Solution solution;
  vector<int> weights = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  int D = 5;

  cout << "Minimum capacity to ship within " << D
       << " days: " << solution.shipWithinDays(weights, D) << endl;
  return 0;
}
