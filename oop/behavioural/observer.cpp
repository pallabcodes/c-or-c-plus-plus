#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>

class Observer; // Forward declaration

class Subject
{
public:
  virtual void registerObserver(Observer *o) = 0;
  virtual void removeObserver(Observer *o) = 0;
  virtual void notifyObservers() = 0;
  virtual ~Subject() = default; // Virtual destructor for proper cleanup
};

class Observer
{
public:
  virtual void update(int value) = 0;
  virtual ~Observer() = default; // Virtual destructor for proper cleanup
};

class ConcreteSubject : public Subject
{
private:
  std::vector<Observer *> observers;
  int value = 0;

public:
  void registerObserver(Observer *o) override
  {
    observers.push_back(o);
  }

  void removeObserver(Observer *o) override
  {
    observers.erase(std::remove(observers.begin(), observers.end(), o), observers.end());
  }

  void notifyObservers() override
  {
    for (Observer *observer : observers)
    {
      observer->update(value);
    }
  }

  void setValue(int val)
  {
    value = val;
    notifyObservers();
  }
};

class ConcreteObserver : public Observer
{
private:
  int value = 0; // Initialize to avoid undefined behavior
  Subject *subject;

public:
  ConcreteObserver(Subject *sSub) : subject(sSub)
  {
    subject->registerObserver(this);
  }

  void update(int val) override
  {
    value = val;
    std::cout << "ConcreteObserver updated with value: " << value << std::endl;
  }
};

/*********** Scenario Implementation ************/

class Customer; // Forward declaration

class Store
{
public:
  virtual void addCustomer(Customer *c) = 0;
  virtual void removeCustomer(Customer *c) = 0;
  virtual void notifyCustomers() = 0;
  virtual void updateQuantity(int quantity) = 0;
  virtual ~Store() = default; // Virtual destructor for proper cleanup
};

class Customer
{
public:
  virtual void update(int stockQuantity) = 0;
  virtual ~Customer() = default; // Virtual destructor for proper cleanup
};

class BookStore : public Store
{
private:
  std::vector<Customer *> customers;
  int stockQuantity = 0;

public:
  void addCustomer(Customer *c) override
  {
    customers.push_back(c);
  }

  void removeCustomer(Customer *c) override
  {
    customers.erase(std::remove(customers.begin(), customers.end(), c), customers.end());
  }

  void notifyCustomers() override
  {
    for (Customer *customer : customers)
    {
      customer->update(stockQuantity);
    }
  }

  void updateQuantity(int quantity) override
  {
    stockQuantity = quantity;
    notifyCustomers();
  }
};

class BookCustomer : public Customer
{
private:
  int observedStockQuantity = 0; // Initialize to avoid undefined behavior
  Store *store;

public:
  BookCustomer(Store *store) : store(store)
  {
    store->addCustomer(this);
  }

  void update(int stockQuantity) override
  {
    observedStockQuantity = stockQuantity;
    if (stockQuantity > 0)
    {
      std::cout << "Hello, A book you are interested in is back in stock!" << std::endl;
    }
  }
};

int main()
{
  std::unique_ptr<Store> store = std::make_unique<BookStore>();

  std::unique_ptr<Customer> customer1 = std::make_unique<BookCustomer>(store.get());
  std::unique_ptr<Customer> customer2 = std::make_unique<BookCustomer>(store.get());

  // Initially, the book is out of stock
  std::cout << "Setting stock to 0." << std::endl;
  store->updateQuantity(0);

  // The book comes back in stock
  std::cout << "Setting stock to 5." << std::endl;
  store->updateQuantity(5);

  // Remove customer1 from the notification list
  store->removeCustomer(customer1.get()); // Use raw pointer to remove customer

  // Simulate the situation where the stock changes again
  std::cout << "\nSetting stock to 2." << std::endl;
  store->updateQuantity(2);

  return 0; // Automatic cleanup of resources due to smart pointers
}
