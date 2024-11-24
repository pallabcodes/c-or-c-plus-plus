// C++ Code: Pass by Value and Pass by Reference

#include <iostream>
using namespace std;

// C++: Pass by Value (Changes inside function do not affect the original variable)
void modifyByValue(int num)
{
  num = 100; // Modifies only the local copy of the variable
}

int main()
{
  int x = 5;

  modifyByValue(x);

  cout << "C++ - x after modifyByValue function call: " << x << endl; // og x remains 5

  return 0;
}

// // C++: Pass by Reference (Changes inside function affect the original variable)
// void modifyByReference(int &num)
// {
//   num = 100; // Modifies the original variable directly
// }

// int main()
// {
//   int x = 5;
//   modifyByReference(x);
//   cout << "C++ - x after modifyByReference function call: " << x << endl; // x becomes 100
//   return 0;
// }
