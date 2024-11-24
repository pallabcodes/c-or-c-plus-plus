#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int largestThree(vector<int> &arr, int target)
{
  int n = arr.size();

  for (int i = 0; i < n; i++)
  {
    if (arr[i] == target)
    {
      return i;
    }
  };

  return -1;
}

int main()
{
  vector<int> arr = {12, 35, 1, 50, 34, 5};
  int result = largestThree(arr, 1);
  if (result == -1)
  {
    cout << "The searched value does not exist" << endl;
  }
  else
  {
    cout << "The value found at " << result << endl;
  };

  return 0;
}
