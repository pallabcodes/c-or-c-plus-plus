#include <iostream>

using namespace std;

/**
 * 1. Whenever a value saved, it saved in memory i.e. RAM but within which components of RAM, STACK OR HEAP (HEAP also known as dynamic memory)
 *
 * N.B: Heap i.e. part of RAM is difference than Heap or Binary Heap data structure, so this HEAP (from RAM) is not an implementation of Binary Heap Data structure but Stack (in RAM) is actually an implementor of STACK DATA STRUCURE
 *
 * 2. The memory i.e. assigned to a program or application, divided into 4 segments i) code/text ii) global iii) stack iv) heap
 *
 * To use dynamic memory with c++, `use new and delete`
 *
 */

int main()
{
  int a; // Variable stored on the stack

  int *p = nullptr; // Initialize pointer to nullptr for safety

  // Allocate memory for a single integer on the heap
  p = new int;
  *p = 10;
  cout << "Value at p: " << *p << endl;

  // Free the allocated memory and reset pointer
  delete p;
  p = nullptr;

  // Allocate memory for an array of 20 integers on the heap
  p = new int[20];

  // Optionally initialize array elements
  for (int i = 0; i < 20; ++i)
  {
    p[i] = i + 1;
  }
  cout << "Array values: ";
  for (int i = 0; i < 20; ++i)
  {
    cout << p[i] << " ";
  }
  cout << endl;

  // Free the allocated memory and reset pointer
  delete[] p;
  p = nullptr;

  return 0;
}

// resource: https://youtu.be/_8-ht2AKyH4
// https: // www.javatpoint.com/cpp-memory-management
// https: // www.javatpoint.com/malloc-vs-new-in-cpp
// https: // www.javatpoint.com/free-vs-delete-in-cpp
// https: // www.youtube.com/watch?v=Dkn4EKL2xSE&ab_channel=AlphaBrainsCourses
// https: // www.youtube.com/watch?v=Dkn4EKL2xSE&ab_channel=AlphaBrainsCourses