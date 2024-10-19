#include <iostream>
#include <memory>

class Lockable
{
public:
  virtual void lock() = 0;
  virtual void unlock() = 0;
  virtual ~Lockable() = default; // Virtual destructor for proper cleanup
};

class NonLocking : public Lockable
{
public:
  void lock() override
  {
    std::cout << "Door does not lock - ignoring" << std::endl;
  }

  void unlock() override
  {
    std::cout << "Door cannot unlock because it cannot lock" << std::endl;
  }
};

class Password : public Lockable
{
public:
  void lock() override
  {
    std::cout << "Door locked using password!" << std::endl;
  }

  void unlock() override
  {
    std::cout << "Door unlocked using password!" << std::endl;
  }
};

class KeyCard : public Lockable
{
public:
  void lock() override
  {
    std::cout << "Door locked using key card!" << std::endl;
  }

  void unlock() override
  {
    std::cout << "Door unlocked using key card!" << std::endl;
  }
};

class Openable
{
public:
  virtual void open() = 0;
  virtual void close() = 0;
  virtual ~Openable() = default; // Virtual destructor for proper cleanup
};

class Standard : public Openable
{
public:
  void open() override
  {
    std::cout << "Pushing door open" << std::endl;
  }

  void close() override
  {
    std::cout << "Pulling door closed" << std::endl;
  }
};

class Revolving : public Openable
{
public:
  void open() override
  {
    std::cout << "Revolving door opened" << std::endl;
  }

  void close() override
  {
    std::cout << "Revolving door closed" << std::endl;
  }
};

class Sliding : public Openable
{
public:
  void open() override
  {
    std::cout << "Sliding door opened" << std::endl;
  }

  void close() override
  {
    std::cout << "Sliding door closed" << std::endl;
  }
};

class Door
{
protected:
  std::unique_ptr<Lockable> lockBehavior; // Use smart pointers for automatic cleanup
  std::unique_ptr<Openable> openBehavior;

public:
  Door() : lockBehavior(nullptr), openBehavior(nullptr) {}

  void setLockBehavior(Lockable *l)
  {
    lockBehavior.reset(l); // Use reset to manage ownership
  }

  void setOpenBehavior(Openable *o)
  {
    openBehavior.reset(o); // Use reset to manage ownership
  }

  void performLock()
  {
    if (lockBehavior)
    {
      lockBehavior->lock();
    }
  }

  void performUnlock()
  {
    if (lockBehavior)
    {
      lockBehavior->unlock();
    }
  }

  void performOpen()
  {
    if (openBehavior)
    {
      openBehavior->open();
    }
  }

  void performClose()
  {
    if (openBehavior)
    {
      openBehavior->close();
    }
  }

  // Implementation of getDimensions() can be added as needed
  void getDimensions()
  {
    // Placeholder for dimensions logic
    std::cout << "Getting dimensions of the door" << std::endl;
  }
};

class ClosetDoor : public Door
{
public:
  // Additional functionalities can be added
};

class ExternalDoor : public Door
{
public:
  // Additional functionalities can be added
};

class SafeDepositDoor : public Door
{
public:
  // Additional functionalities can be added
};

class SlidingDoor : public Door
{
public:
  // Additional functionalities can be added
};

int main()
{
  std::unique_ptr<Door> c = std::make_unique<ClosetDoor>();

  c->setOpenBehavior(new Standard());
  c->setLockBehavior(new NonLocking());

  c->performOpen();
  c->performClose();

  c->performLock();
  c->performUnlock();

  // Upgrade the door to a password protected door
  c->setLockBehavior(new Password());
  c->performLock();
  c->performUnlock();

  return 0;
}
