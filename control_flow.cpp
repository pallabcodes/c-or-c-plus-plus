#include <iostream>

using namespace std;

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

int main()
{
  int num = 10;
  if (num % 2 == 0)
  {
    cout << "It is even number" << endl;
  }
  else
  {
    cout << "It is odd number" << endl;
  }

  int digit = 10;

  switch (num)
  {
  case 10:
    cout << "It is 10";
    break;
  case 20:
    cout << "It is 20";
    break;
  case 30:
    cout << "It is 30";
    break;
  default:
    cout << "Not 10, 20 or 30";
    break;
  }

  return 0;
}