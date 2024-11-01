#include <iostream>
using namespace std;

int main()
{

  // int *ptr;
  // float f = 10.3;
  // ptr = &f; // error
  // cout << "The value of *ptr is : " << *ptr << endl;

  void *ptr; // void pointer declaration
  int a = 9; // integer variable initialization
  ptr = &a;  // storing the address of 'a' variable in a void pointer variable.
  std::cout << &a << std::endl;
  std::cout << ptr << std::endl;

  void *vptr;                                            // void pointer declaration
  int *iptr1;                                            // integer pointer declaration
  int data = 10;                                         // integer variable initialization
  vptr = &data;                                          // storing the address of data variable in void pointer variable
  iptr1 = (int *)vptr;                                   // assigning void pointer to integer pointer but it has typecasted like done here
  cout << "The value of *iptr1 is : " << *iptr1 << endl; // Dereferences iptr1 to get the original value of 'data'

  return 0;
}