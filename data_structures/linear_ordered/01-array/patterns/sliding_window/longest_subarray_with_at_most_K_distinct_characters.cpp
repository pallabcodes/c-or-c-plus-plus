#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

int totalFruit(vector<int> &fruits) {
  unordered_map<int, int> basket;
  int left = 0, maxLen = 0;

  for (int right = 0; right < fruits.size(); right++) {
    basket[fruits[right]]++;

    while (basket.size() > 2) { // Shrink window if more than 2 types
      basket[fruits[left]]--;
      if (basket[fruits[left]] == 0)
        basket.erase(fruits[left]);
      left++;
    }

    maxLen = max(maxLen, right - left + 1);
  }

  return maxLen;
}

int main() {
  vector<int> fruits = {1, 2, 1, 2, 3};
  cout << totalFruit(fruits) << endl; // Output: 4
  return 0;
}
