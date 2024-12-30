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

    virtual void displayInfo() const = 0;           // pure virtual function
    virtual double calculateRentalCost() const = 0; // pure virtual function

    virtual ~Vehicle() {} // Virtual destructor
};

// Derived class
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

    ~RentalAgency()
    {
        for (auto &vehicle : inventory)
        {
            delete vehicle; // Clean up dynamically allocated memory
        }
    }
};

// Abstract class
class AbstractEmployee
{
public:
    virtual void askForPermission() = 0; // Pure virtual function
};

// Derived class
class Student : public AbstractEmployee
{
public:
    string name;

private:
    string address;
    int rollNo;
    string dept;
    int age;

public:
    Student(string name, string address, int rollNo, string dept, int age)
        : name(name), address(address), rollNo(rollNo), dept(dept), age(age) {}

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

    void introduceYourself()
    {
        cout << "Hello, My name is " << name << endl;
    }

    void setName(string name)
    {
        this->name = name;
    }

    string getName()
    {
        return name;
    }
};

// Derived class
class Developer : public Student
{
public:
    string favoriteProgrammingLang;

    Developer(string name, string address, int rollNo, string dept, string favoriteProgrammingLang, int age)
        : Student(name, address, rollNo, dept, age), favoriteProgrammingLang(favoriteProgrammingLang) {}

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

    // Adding vehicles to the inventory
    agency.addVehicle(new Car("Toyota", "Camry", 2022, 4));
    agency.addVehicle(new Car("Honda", "Civic", 2023, 2));
    agency.addVehicle(new Motorcycle("Harley-Davidson", "Street 750", 2021, false));
    agency.addVehicle(new Motorcycle("BMW", "R1250GS", 2023, true));

    cout << "Rental Agency Inventory:" << endl;
    agency.displayInventory();

    cout << "\nTotal rental cost for all vehicles: $" << agency.calculateTotalRentalCost() << endl;

    return 0;
}