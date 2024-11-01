#include <iostream>

using namespace std;

void displayPointerValues()
{

  // N.B: When you assign ptr = a;, ptr points to the first element of the array, meaning *ptr will give you the value of a[0], which is 1 in this case.

  int a[] = {1, 2, 3, 4, 5}; // array initialization
  int *ptr;                  // pointer declaration
  // assigning base address to pointer ptr we mean the memory address of the first element of the array, which is a[0].
  ptr = a;

  cout << "The value of *ptr is: " << *ptr;
  // 1. Initially, ptr points to a[0], which contains the value 1.

  // 2. When you increment the pointer with ptr + 1, it moves the pointer to the next memory location, which corresponds to the second element of the array :

  //  When you increment the pointer with ptr + 1, it moves the pointer to the next memory location, which corresponds to the second element of the array :

  // ptr = ptr + 1; now makes ptr point to a[1].

  // Therefore, *ptr after this increment will give you the value of a[1], which is 2.

  // So yes, ptr + 1 is accessing the second value in the array, a[1].

  // Base address of array 'a' points to a[0]; 'ptr + 1' moves 'ptr' to a[1], accessing the second element.
  ptr = ptr + 1; // incrementing the value of ptr by 1
  cout << "\nThe value of *ptr is: " << *ptr << endl;
}

// C++ reference and pointer seem to be similar, but there are some differences that exist between them. A reference is a variable which is another name of the existing variable, while the pointer is variable that stores the address of another variable.

// A reference variable is another name for an already existing variable that holds the memory address of that exisiting variable.

// KEY DIFFERENCE: Reference is a variable that contains / stores the variable whereas pointer is also off course a variable that stored a memory address of the variable

void func(int &m)
{
  m = 8;
}

int main()
{
  int ii = 8;   // variable initialization
  int &aa = ii; // creating a reference variable
  cout << "The value of 'i' variable is :" << aa;

  int x = 10;
  cout << "Value of 'x' is :" << x << endl;
  func(x);
  cout << "Now value of 'x' is :" << x << endl;

  // N.B: We cannot reassign the reference variable
  int j; // variable declaration
  int k; // variable declaration
  int &m = j;
  // int &m = k; // error as a reference vairable can not be `reassigned`

  // # Why a Reference Cannot Be Reassigned

  // Since a is a reference to i, both &a and &i will print the same memory address.This demonstrates that a is not an independent variable but merely an alias to i.

  // Once a reference is initialized to a variable, it cannot be reassigned to refer to a different variable. The reference a will always point to i until i goes out of scope. If i is destroyed (e.g., if it's a local variable and goes out of scope), a would refer to an invalid address, which can lead to undefined behavior if accessed.

  int e;
  int &f = e;
  cout << "The address of 'e' variable is : " << &e << endl;
  cout << "The address of 'f' variable is : " << &f << endl;

  // N.B: We cannot assign the NULL value to the `reference variable`, but the `pointer variable` can be assigned with a NULL value.

  int *p;    // Declares 'p' as a pointer to an int
  int a = 8; // Declares 'a' as an int with value 8

  // q is meant to store the address of a pointer(p in this case), which itself points to an integer(a).

  // q is an int (pointer to pointer variable) **that stores the address of p.

  // q i.e. a pointer is simply stores the address of a pointer (p in this case) which is why it's a pointer to pointer variable

  int **q; // Declares 'q' as a pointer to an int pointer

  // q is a pointer to a pointer, meaning it stores the address of another pointer variable.
  // Similarly, a triple pointer would store the address of a pointer-to-pointer variable.

  p = &a; // Assigns 'p' to the address of 'a'
  q = &p; // Assigns 'q' to the address of 'p'

  cout << "The value of q is : " << *q << endl; // Prints the address stored in 'p' (address of 'a')

  // N.B: unlike a pointer ot pointer variable, a "Reference to reference variable is not valid"

  // int digit = 8;   // variable initialization
  // int &p = digit;  // creating a reference variable for ?a? variable.
  // int &&q = digit; // reference to reference is not valid, it throws an error. unlike the pointer to pointer variable

  displayPointerValues();

  // int value = 90; // variable declaration
  // int &a = value; // assigning value to the reference
  // &a = &a + 5     // arithmetic operation (here addition) is not possible with reference variable, it throws an error.

  return 0;
}
