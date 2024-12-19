#include <iostream>

using namespace std;

// C++ Struct Example (although struct can hold both data and methods, a struct should ideally only hold data/fields)
struct Person
{
  string Name;
  int Age;

  void Introduce()
  {
    cout << "Hello, my name is " << Name << " and I'm " << Age << " years old." << std::endl;
  }
};

// C++ Interface Example (using an abstract class instead of a formal interface, as it does not implement the methods)
class ISpeaker
{
public:
  virtual void Speak() = 0;      // Pure virtual function makes this class abstract
  virtual ~ISpeaker() = default; // Virtual destructor for safe polymorphic deletion
};

// Concrete class implementing the ISpeaker interface
class Speaker : public ISpeaker
{
public:
  void Speak() override
  {
    cout << "Speaking..." << std::endl;
  }
};

int main()
{
  // Creating a Person struct instance
  Person person;
  person.Name = "John Doe";
  person.Age = 30;
  person.Introduce(); // Output: Hello, my name is John Doe and I'm 30 years old.

  // Creating a Speaker object and calling the Speak method
  Speaker speaker;
  speaker.Speak(); // Output: Speaking...

  return 0;
}
