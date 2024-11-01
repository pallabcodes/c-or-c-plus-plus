#include <iostream>

using namespace std;

// ## Array of pointers
void processArray()
{
  int ptr1[5];  // Declare an array of 5 integers, e.g., [1, 2, 3, 4, 5]
  int *ptr2[5]; // Declare an array of 5 integer pointers, e.g., [&num1, &num2, &num3, &num4, &num5]

  cout << "Enter five numbers :" << endl;

  // Read 5 numbers into the ptr1 array
  for (int i = 0; i < 5; i++)
  {
    // a) Prompts the user to enter a number.
    // b) Stores the entered number in the current position of the array (ptr1[i]).
    // N.B: ">> Operator: This is the stream extraction operator". It takes the input from cin and stores it in the variable on the right side, which in this case is ptr1[i].
    cin >> ptr1[i]; // Example input: [10, 20, 30, 40, 50]
  }

  // Assign the address of each element in ptr1 to the corresponding element in ptr2
  for (int i = 0; i < 5; i++)
  {
    // Since ptr2 will only take integer pointers, it must store integer addresses.
    ptr2[i] = &ptr1[i]; // Example addresses: [&10, &20, &30, &40, &50]
  }

  // Print the values of the ptr1 array using the pointers in ptr2
  cout << "The values are" << endl;

  for (int i = 0; i < 5; i++)
  {
    // Dereference the pointer to get the value stored at that address
    cout << "dereference to get the value " << *ptr2[i] << endl;
  }
}

// ## Array of Pointer to Strings

void printNames()
{
  // Declare an array of constant character pointers
  const char *names[] = {
      "John",
      "Peter",
      "Marco",
      "Devin",
      "Ronan"};

  for (int i = 0; i < 5; i++)
  {
    cout << names[i] << endl;
  }
}

int main()
{
  // int *ptr = nullptr; // Pointer initialized to nullptr

  // int marks[10] = {}; // Array of 10 integers initialized to 0

  // cout << "Enter the elements of an array: " << endl;

  // for (int i = 0; i < 10; i++)
  // {
  //   cin >> marks[i];
  // }

  /*

  In C++, when you assign an array to a pointer, the pointer points to the first element of the array. This is because the name of the array (marks in this case) is essentially a pointer to its first element.

  Here's a breakdown of the behavior:
  ptr = marks; assigns the address of the first element of marks to ptr. So, ptr now points to the first element of the array.

  *ptr dereferences ptr, giving you the value of the first element of the array.

  *marks also gives you the value of the first element of the array because marks is equivalent to &marks[0].

  Therefore, both *ptr and *marks will output the value of the first element of the marks array. The cout statements in your code will print the same value for both *ptr and *marks.

  */

  // ptr = marks; // both marks and ptr pointing to the same element i.e. first element of the array

  // std::cout << "The value of *ptr is :" << *ptr << std::endl;     // Output the value pointed to by ptr
  // std::cout << "The value of *marks is :" << *marks << std::endl; // Output the value of the first element of marks

  // processArray();

  printNames();

  return 0;
}