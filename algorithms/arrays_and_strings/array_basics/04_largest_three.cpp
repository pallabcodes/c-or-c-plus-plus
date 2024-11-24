#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

vector<int> getLeaders(vector<int> &arr)
{
  int n = arr.size();

  if (n < 3)
  {
    return {}; // Return empty if not enough elements for three largest values.
  }

  int x = INT_MIN, y = INT_MIN, z = INT_MIN; // largest, secondLargest, thirdLargest

  for (int i = 0; i < n; i++)
  {
    // Check if current element should be the largest
    if (arr[i] > x)
    {
      z = y;      // Shift second largest to third
      y = x;      // Shift largest to second
      x = arr[i]; // Update largest
    }
    // Check if current element should be the second largest
    else if (arr[i] > y)
    {
      z = y;      // Shift second largest to third
      y = arr[i]; // Update second largest
    }
    // Check if current element should be the third largest
    else if (arr[i] > z)
    {
      z = arr[i]; // Update third largest
    }
  }

  return {x, y, z};
}

int main()
{
  vector<int> arr = {12, 35, 1, 50, 34, 5};
  vector<int> result = getLeaders(arr);

  cout << "The largest three are: ";
  for (int num : result)
  {
    cout << num << " ";
  }
  cout << endl;

  return 0;
}
