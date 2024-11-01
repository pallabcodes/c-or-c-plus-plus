#include <iostream>   // Includes the standard input/output stream library
#include <functional> // Includes the functional library for using std::function
using namespace std;  // Allows usage of standard library names without prefixing with std::

void func1()
{                              // Defines a function named func1 that takes no parameters
  cout << "func1 is called\n"; // Outputs a message indicating that func1 has been called
}

void func2(const function<void()> &func)
{
  func(); // Calls the function referenced by func
}

int main()
{               // Entry point of the program
  func2(func1); // Calls func2 and passes func1 as an argument
  return 0;     // Returns 0 to indicate successful completion of the program
}
