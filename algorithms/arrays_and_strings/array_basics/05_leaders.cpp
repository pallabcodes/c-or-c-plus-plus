#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

// Brute-force: O(n^2), space: O(1)
vector<int> getLeadersBruteForce(vector<int> &arr)
{
  int n = arr.size();
  vector<int> res;

  for (int i = 0; i < n; i++)
  {
    bool isLeader = true;

    for (int j = i + 1; j < n; j++)
    {
      if (arr[i] <= arr[j])
      {
        isLeader = false;
        break;
      }
    }

    if (isLeader)
    {
      res.push_back(arr[i]);
    }
  }

  return res;
}

// Optimized: O(n), space: O(1)
vector<int> getLeadersOptimized(vector<int> &arr)
{
  int n = arr.size();
  vector<int> res;
  int maxFromRight = INT_MIN;

  for (int i = n - 1; i >= 0; i--)
  {
    if (arr[i] >= maxFromRight)
    {
      res.push_back(arr[i]);
      maxFromRight = arr[i];
    }
  }

  reverse(res.begin(), res.end()); // Reverse to maintain original order
  return res;
}

// Wrapper: Select brute-force or optimized
vector<int> getLeaders(vector<int> &arr, bool optimized = true)
{
  if (optimized)
  {
    return getLeadersOptimized(arr);
  }
  else
  {
    return getLeadersBruteForce(arr);
  }
}

int main()
{
  vector<int> arr = {12, 10, 8, 5, 4, 6};

  // Use optimized version by default
  vector<int> result = getLeaders(arr);

  cout << "The leaders in the array are: ";
  for (auto leader : result)
  {
    cout << leader << " ";
  }
  cout << endl;

  return 0;
}
