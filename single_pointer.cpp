#include <iostream>

using namespace std;

int main()
{

  int num = 42;
  int *ptr = &num;

  cout << ptr << " -- " << &num << " must be the same" << endl;

  cout << *ptr << endl;

  *ptr = 100; // Modify num using the pointer

  cout << "Modified num: " << num << " " << *ptr << " " << endl; // 100

  return 0;
}