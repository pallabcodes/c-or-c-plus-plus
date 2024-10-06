#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Abstract Base class
class Vehicle
{
protected:
  string make;
  string model;
  int year;

public:
  Vehicle(string make, string model, int year) : make(make), model(model), year(year) {}

  // N.B: These pure virtual functions must be implemented by derived classes.
  virtual void displayInfo() const = 0;           // pure virtual function
  virtual double calculateRentalCost() const = 0; // pure virtual function

  // Virtual destructor
  // N.B: This is crucial when dealing with polymorphism and ensures that the correct destructor is called when deleting a derived class object through a base class pointer.

  // Elaborate: Why need to use virtual destructors below on this class ?

  // 1. Polymorphic Deletion: In our RentalAgency class, we store pointers to Vehicle objects: vector<Vehicle *> inventory; Now, These pointers might actually point to Car or Motorcycle objects (derived classes).

  // 2. Destructor Call: When we delete these objects in the RentalAgency destructor e.g. ~RentalAgency : The delete operation needs to know which destructor to call.

  // 3. Without Virtual Destructor: If ~Vehicle() wasn't virtual, only the Vehicle destructor would be called, even if vehicle pointed to a Car or Motorcycle. This could lead to partial cleanup and memory leaks.

  // 4. With Virtual Destructor: Because ~Vehicle() is virtual, the correct destructor is called based on the actual type of the object: If vehicle points to a Car, Car's destructor is called, then Vehicle's. If vehicle points to a Motorcycle, Motorcycle's destructor is called, then Vehicle's.

  // 5. Real-world Impact: In our current example, Car and Motorcycle don't have explicit destructors or additional resources to clean up. But in a more complex scenario, they might. For example:

  //   class Car : public Vehicle {
  //     char* specialEquipment;
  // public:
  //     Car(...) {
  //         specialEquipment = new char[100];
  //         // ... initialize ...
  //     }
  //     ~Car() {
  //         delete[] specialEquipment; // clean up the specialEquipment when this class is destroyed/deleted
  //     }
  // };

  // 6. Ensuring Proper Cleanup: The virtual destructor ensures that no matter how we delete a Vehicle-derived object, all the necessary cleanup code (from the most derived class up through the inheritance chain) is executed.

  // summary: In our current code, while we don't have complex destructors in Car or Motorcycle, the virtual destructor in Vehicle is a good practice. It future-proofs our code, allowing us to add specific cleanup code to derived classes later if needed, without worrying about potential memory leaks or improper cleanup.

  virtual ~Vehicle() {}
};

// Car and Motorcycle are derived classes that inherit from Vehicle and provide concrete implementations of the pure virtual functions

// derived class
class Car : public Vehicle
{
private:
  int numOfDoors;

public:
  Car(string make, string model, int year, int doors)
      : Vehicle(make, model, year), numOfDoors(doors) {}

  void displayInfo() const override
  {
    cout << year << " " << make << " " << model << " (Car, " << numOfDoors << " doors)" << endl;
  }

  double calculateRentalCost() const override
  {
    return 50.0 * numOfDoors; // Base rate of $50 per door
  }
};

// Another derived class
class Motorcycle : public Vehicle
{
private:
  bool hasSidecar;

public:
  Motorcycle(string make, string model, int year, bool sidecar)
      : Vehicle(make, model, year), hasSidecar(sidecar) {}

  void displayInfo() const override
  {
    cout << year << " " << make << " " << model << " (Motorcycle, "
         << (hasSidecar ? "with" : "without") << " sidecar)" << endl;
  }

  double calculateRentalCost() const override
  {
    return hasSidecar ? 80.0 : 60.0; // $80 with sidecar, $60 without
  }
};

// Rental agency class demonstrating encapsulation
class RentalAgency
{
private:
  // inventory is a vector but specifically a vector of Vehicle objects. This allows it to store pointers of both Car and Motorcycle objects due to polymorphism. Since, a Vehicle must have attributes within its sub-class i.e. both Car and Motorcycle

  // why used pointer on Vehicle i.e. Vehicle * ?

  // 1. Polymorphism: We can store pointers to derived class objects (Car*, Motorcycle*) in a container of base class pointers (Vehicle*).
  // 2. Dynamic allocation: Objects created with new have a lifetime that extends beyond the scope they're created in, allowing for more flexible memory management.

  vector<Vehicle *> inventory;

public:
  void addVehicle(Vehicle *vehicle)
  {
    inventory.push_back(vehicle);
  }

  void displayInventory() const
  {
    for (const auto &vehicle : inventory)
    {
      vehicle->displayInfo();
    }
  }

  double calculateTotalRentalCost() const
  {
    double total = 0.0;
    for (const auto &vehicle : inventory)
    {
      total += vehicle->calculateRentalCost();
    }
    return total;
  }

  // ~ symbol denotes a destructor
  // A destructor is automatically called when an object goes out of scope or is explicitly deleted.
  // Its purpose is to clean up any resources that the object may have acquired during its lifetime.

  // This destructor iterates through all vehicles (regardless of whether Car or Motorcycle) in the inventory and deletes each one.
  // The delete keyword frees the memory that was allocated with new.
  // This ensures that when a RentalAgency object is destroyed, it cleans up all the Vehicle objects it was managing, preventing memory leaks.
  ~RentalAgency()
  {
    for (auto &vehicle : inventory)
    {
      delete vehicle; // This is where magic happens
    }
  }
};

// This is an abstract class

class AbstractEmployee
{
public:
  // Abstract method that derived classes must implement
  virtual void askForPermission() = 0; // Pure virtual function
};

// So, here Student inherits from the AbstractEmployee
class Student : public AbstractEmployee
{
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
  void askForPermission() override
  {
    if (age > 30)
    {
      cout << "Getting promoted" << endl;
    }
    else
    {
      cout << "Negotiating" << endl;
    }
  }

  void introduceYourself() { cout << "Hello, My name is " << name << endl; }

  // Setter
  void setName(string name) { this->name = name; }

  // Getter
  string getName() { return name; }
};

class Developer : public Student
{
public:
  string favoriteProgrammingLang;

  // Constructor
  Developer(string name, string address, int rollNo, string dept,
            string favoriteProgrammingLang, int age)
      : Student(name, address, rollNo, dept, age),
        favoriteProgrammingLang(favoriteProgrammingLang) {}

  void fixBug()
  {
    cout << name << " fixed the bug using " << favoriteProgrammingLang << endl;
  }
};

int main()
{
  // Creating instances of Student
  Student student1("John", "Boston", 30, "Wrestling", 29);
  Student student2("Jose", "Madrid", 20, "Football", 25);

  // Demonstrating functionality
  student1.askForPermission();
  student2.introduceYourself();

  // Creating an instance of Developer
  Developer developer("Johnson", "UK", 40, "Engineering", "C++", 35);
  developer.fixBug();

  RentalAgency agency;

  // N.B: When we add vehicles to the inventory, we're storing pointers to dynamically allocated objects:
  // The new keyword allocates memory on the heap and returns a pointer to the object.
  // These dynamically allocated objects need to be manually deleted to prevent memory leaks.

  agency.addVehicle(new Car("Toyota", "Camry", 2022, 4));
  agency.addVehicle(new Car("Honda", "Civic", 2023, 2));
  agency.addVehicle(new Motorcycle("Harley-Davidson", "Street 750", 2021, false));
  agency.addVehicle(new Motorcycle("BMW", "R1250GS", 2023, true));

  cout << "Rental Agency Inventory:" << endl;
  agency.displayInventory();

  cout << "\nTotal rental cost for all vehicles: $" << agency.calculateTotalRentalCost() << endl;

  return 0;
}

// Inheritance: Both Car and Motorcycle inherits from the Vehicle base class

// Polymorphism: The displayInfo() and calculateRentalCost() functions are polymorphic, with different implementations in each derived class.

// Abstraction: The Vehicle class is an abstract base class with pure virtual functions, representing the concept of a vehicle without specifying all details.

// Encapsulation: The RentalAgency class encapsulates the vehicle inventory and provides a public interface to interact with it.