#include <iostream>
#include <memory>

enum class Starter
{
  SALAD,
  SOUP,
  BRUSCHETTA,
  VEGGIE_STICKS,
  CHICKEN_WINGS,
};

enum class Main
{
  GRILLED_CHICKEN,
  PASTA,
  VEGGIE_STIR_FRY,
  FISH,
  PIZZA,
};

enum class Dessert
{
  FRUIT_SALAD,
  ICE_CREAM,
  CHOCOLATE_CAKE,
  VEGAN_PUDDING,
  CHEESECAKE,
};

enum class Drink
{
  WATER,
  VEGAN_SHAKE,
  SODA,
  FRUIT_JUICE,
};

class Meal
{
private:
  Starter starter;
  Main main;
  Dessert dessert;
  Drink drink;

public:
  Starter getStarter() const { return starter; }
  Main getMain() const { return main; }
  Dessert getDessert() const { return dessert; }
  Drink getDrink() const { return drink; }

  void setStarter(Starter s) { starter = s; }
  void setMain(Main m) { main = m; }
  void setDessert(Dessert d) { dessert = d; }
  void setDrink(Drink d) { drink = d; }
};

class Builder
{
public:
  virtual ~Builder() {}
  virtual void addStarter() = 0;
  virtual void addMainCourse() = 0;
  virtual void addDessert() = 0;
  virtual void addDrink() = 0;
  virtual Meal build() = 0;
};

class VeganMealBuilder : public Builder
{
private:
  Meal meal;

public:
  void addStarter() override { meal.setStarter(Starter::SALAD); }
  void addMainCourse() override { meal.setMain(Main::VEGGIE_STIR_FRY); }
  void addDessert() override { meal.setDessert(Dessert::VEGAN_PUDDING); }
  void addDrink() override { meal.setDrink(Drink::VEGAN_SHAKE); }
  Meal build() override { return meal; }
};

class HealthyMealBuilder : public Builder
{
private:
  Meal meal;

public:
  void addStarter() override { meal.setStarter(Starter::SALAD); }
  void addMainCourse() override { meal.setMain(Main::GRILLED_CHICKEN); }
  void addDessert() override { meal.setDessert(Dessert::FRUIT_SALAD); }
  void addDrink() override { meal.setDrink(Drink::WATER); }
  Meal build() override { return meal; }
};

class Director
{
public:
  void constructVeganMeal(Builder &builder)
  {
    builder.addStarter();
    builder.addMainCourse();
    builder.addDessert();
    builder.addDrink();
  }

  void constructHealthyMeal(Builder &builder)
  {
    builder.addStarter();
    builder.addMainCourse();
    builder.addDessert();
    builder.addDrink();
  }
};

int main()
{
  Director director;
  VeganMealBuilder veganBuilder;
  director.constructVeganMeal(veganBuilder);

  Meal veganMeal = veganBuilder.build();
  std::cout << "Vegan Meal constructed." << std::endl;

  HealthyMealBuilder healthyBuilder;
  director.constructHealthyMeal(healthyBuilder);
  Meal healthyMeal = healthyBuilder.build();
  std::cout << "Healthy Meal constructed." << std::endl;

  return 0;
}