#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int getSecondLargest(vector<int> &arr)
{
  int n = arr.size();

  int largest = -1, secondLargest = -1;

  for (int i = 0; i < n; i++)
  {
    // current element i.e. arr[i] > largest, then largest = arr[i]; second_largest = largest
    if (arr[i] > largest)
    {
      secondLargest = largest;
      largest = arr[i];
    }
    else if (arr[i] < largest && arr[i] > secondLargest)
    {
      secondLargest = arr[i];
    }
  }

  return secondLargest;
}

int main()
{
  vector<int> arr = {12, 35, 1, 10, 34, 5};
  cout << getSecondLargest(arr);

  return 0;
}
