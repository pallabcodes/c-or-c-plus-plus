#include <iostream>

using namespace std;

#include <iostream>

// Array traversal

int main()
{
  int arr[5] = {1, 2, 3, 4, 5}; // Fixed-size array of 5 elements

  // Calculate length of the array
  size_t length = sizeof(arr) / sizeof(arr[0]);

  std::cout << "Length of the array: " << length << std::endl;

  return 0;
}

int nestedLoop()
{
  for (int i = 1; i <= 3; i++)
  {
    for (int j = 1; j <= 3; j++)
    {
      cout << i << " " << j << "\n";
    }
  }
}

int nestedWhileLoop()
{
  int i = 1;
  while (i <= 3)
  {
    int j = 1;
    while (j <= 3)
    {
      cout << i << " " << j << "\n";
      j++;
    }
    i++;
  }
}

int doWhile()
{
  int i = 1;
  do
  {
    cout << i << "\n";
    i++;
  } while (i <= 10);
}

int doWhileNested()
{
  int i = 1;
  do
  {
    int j = 1;
    do
    {
      cout << i << "\n";
      j++;
    } while (j <= 3);
    i++;
  } while (i <= 3);
}

int main()
{
  int i = 1;
  while (i <= 10)
  {
    cout << i << "\n";
    i++;
  }

  nestedLoop();

  nestedWhileLoop();

  return 0;
}