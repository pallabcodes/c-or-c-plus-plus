#include <cmath>
#include <iostream>
#include <string>
// #include <vector>

using namespace std;

// Abstract Base class for an Employee
class Employee {
  int id;
  string name;
  double salary;

public:
  Employee(string name, int id, double salary)
      : name(name), id(id), salary(salary) {}
};

// This is an abstract class

class AbstractEmployee {
public:
  // Abstract method that derived classes must implement
  virtual void askForPermission() = 0; // Pure virtual function
};

// So, here Student inherits from the AbstractEmployee
class Student : public AbstractEmployee {
public:
  string name;

private:
  string address;
  int rollNo;
  string dept;
  int age; // Added age for the permission logic

public:
  // Constructor
  Student(string name, string address, int rollNo, string dept, int age)
      : name(name), address(address), rollNo(rollNo), dept(dept), age(age) {}

  // Implementation of the abstract method
  void askForPermission() override {
    if (age > 30) {
      cout << "Getting promoted" << endl;
    } else {
      cout << "Negotiating" << endl;
    }
  }

  void introduceYourself() { cout << "Hello, My name is " << name << endl; }

  // Setter
  void setName(string name) { this->name = name; }

  // Getter
  string getName() { return name; }
};

class Developer : public Student {
public:
  string favoriteProgrammingLang;

  // Constructor
  Developer(string name, string address, int rollNo, string dept,
            string favoriteProgrammingLang, int age)
      : Student(name, address, rollNo, dept, age),
        favoriteProgrammingLang(favoriteProgrammingLang) {}

  void fixBug() {
    cout << name << " fixed the bug using " << favoriteProgrammingLang << endl;
  }
};

int main() {
  // Creating instances of Student
  Student student1("John", "Boston", 30, "Wrestling", 29);
  Student student2("Jose", "Madrid", 20, "Football", 25);

  // Demonstrating functionality
  student1.askForPermission();
  student2.introduceYourself();

  // Creating an instance of Developer
  Developer developer("Johnson", "UK", 40, "Engineering", "C++", 35);
  developer.fixBug();

  return 0;
}
