#include <iostream>
using namespace std;

// N.B: when a pointer needs to be modified, then that pointer variable e.g. *ptr uses double pointers e.g. **ptr as below

void modifyPointer(int **ptr)
{
  static int newNum = 500; // Use static to ensure lifetime during function execution
  *ptr = &newNum;          // Change where the original pointer points
}

int main()
{
  int num = 42;
  int *ptr = &num;
  cout << "Original value: " << *ptr << endl; // 42

  modifyPointer(&ptr);                        // Passing the address of the pointer
  cout << "Modified value: " << *ptr << endl; // 500

  return 0;
}
