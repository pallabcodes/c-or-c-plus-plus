#include <iostream>
#include <string>

class Beverage
{
public:
  virtual ~Beverage() = default;
  virtual double cost() const = 0;
  virtual std::string description() const = 0;
};

class DarkRoast : public Beverage
{
public:
  double cost() const override
  {
    return 3.45;
  }
  std::string description() const override
  {
    return "Dark Roast";
  }
};

class LightRoast : public Beverage
{
public:
  double cost() const override
  {
    return 3.45;
  }
  std::string description() const override
  {
    return "Light Roast";
  }
};

class BeverageDecorator : public Beverage
{
protected:
  Beverage *beverage;

public:
  BeverageDecorator(Beverage *b) : beverage(b) {}
  virtual ~BeverageDecorator()
  {
    delete beverage;
  }
};

class EspressoDecorator : public BeverageDecorator
{
public:
  EspressoDecorator(Beverage *b) : BeverageDecorator(b) {}
  double cost() const override
  {
    return 0.5 + beverage->cost();
  }
  std::string description() const override
  {
    return beverage->description() + ", Espresso";
  }
};

class CreamDecorator : public BeverageDecorator
{
public:
  CreamDecorator(Beverage *b) : BeverageDecorator(b) {}
  double cost() const override
  {
    return 0.3 + beverage->cost();
  }
  std::string description() const override
  {
    return beverage->description() + ", Cream";
  }
};

class FoamDecorator : public BeverageDecorator
{
public:
  FoamDecorator(Beverage *b) : BeverageDecorator(b) {}
  double cost() const override
  {
    return 0.2 + beverage->cost();
  }
  std::string description() const override
  {
    return beverage->description() + ", Foam";
  }
};

int main()
{
  Beverage *beverage = new FoamDecorator(
      new CreamDecorator(new EspressoDecorator(new LightRoast())));
  std::cout << beverage->description() << std::endl; // Outputs: Light Roast, Espresso, Cream, Foam
  std::cout << beverage->cost() << std::endl;        // Outputs: 4.45

  delete beverage;
  return 0;
}