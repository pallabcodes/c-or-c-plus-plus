#include <iostream>
#include <string>
#include <vector>

enum class Burgers
{
    CHEESE,
    DELUXECHEESE,
    VEGAN,
    DELUXEVEGAN
};

class Burger
{
public:
    virtual void prepare() {}
    virtual void cook() {}
    virtual void serve() {}
    std::string getName() { return name; }

protected:
    std::string name;
    std::string bread;
    std::string sauce;
    std::vector<std::string> toppings;
};

class CheeseBurger : public Burger
{
public:
    CheeseBurger()
    {
        name = "Cheese Burger";
        // ... set the name, bread, and sauce
    }
};

class DeluxeCheeseBurger : public Burger
{
public:
    DeluxeCheeseBurger()
    {
        name = "Deluxe Cheese Burger";
        // ... set the name, bread, and sauce
    }
};

class VeganBurger : public Burger
{
public:
    VeganBurger()
    {
        name = "Vegan Burger";
        // ... set the name, bread, and sauce
    }
};

class DeluxeVeganBurger : public Burger
{
public:
    DeluxeVeganBurger()
    {
        name = "Deluxe Vegan Burger";
        // ... set the name, bread, and sauce
    }
};

class BurgerStore
{
public:
    virtual Burger *createBurger(Burgers item) = 0;

    Burger *orderBurger(Burgers type)
    {
        Burger *burger = createBurger(type);
        std::cout << "--- Making a " << burger->getName() << " ---" << std::endl;
        burger->prepare();
        burger->cook();
        burger->serve();
        return burger;
    }
};

class CheeseBurgerStore : public BurgerStore
{
public:
    Burger *createBurger(Burgers item) override
    {
        if (item == Burgers::CHEESE)
        {
            return new CheeseBurger();
        }
        else if (item == Burgers::DELUXECHEESE)
        {
            return new DeluxeCheeseBurger();
        }
        else
        {
            return nullptr;
        }
    }
};

class VeganBurgerStore : public BurgerStore
{
public:
    Burger *createBurger(Burgers item) override
    {
        if (item == Burgers::VEGAN)
        {
            return new VeganBurger();
        }
        else if (item == Burgers::DELUXEVEGAN)
        {
            return new DeluxeVeganBurger();
        }
        else
        {
            return nullptr;
        }
    }
};

int main()
{
    BurgerStore *cheeseStore = new CheeseBurgerStore();
    BurgerStore *veganStore = new VeganBurgerStore();

    Burger *burger = cheeseStore->orderBurger(Burgers::CHEESE);
    std::cout << "Ethan ordered a " << burger->getName() << std::endl;

    burger = veganStore->orderBurger(Burgers::DELUXEVEGAN);
    std::cout << "Joel ordered a " << burger->getName() << std::endl;

    delete cheeseStore;
    delete veganStore;
    delete burger;

    return 0;
}