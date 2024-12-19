#include <stdio.h> // Corrected header file

// C Struct Example (unlike C++ or C#, here struct only holds fields as intended)
struct Person
{
  char Name[50];
  int Age;
};

// C "Interface" using function pointers (not a formal interface)
struct Speaker
{
  void (*speak)(void); // Function pointer to a function that takes no arguments and returns nothing
};

// Function that implements the speak functionality
void speakFunction()
{
  printf("Speaking...\n");
}

int main()
{
  // Creating a Person struct instance
  struct Person person;
  snprintf(person.Name, sizeof(person.Name), "John Doe"); // Safely copy string to Name
  person.Age = 30;

  // Printing out the person's details
  printf("Hello, my name is %s and I'm %d years old.\n", person.Name, person.Age);

  // Creating a Speaker struct instance and assigning the speak function
  struct Speaker speaker;
  speaker.speak = speakFunction; // Assigning function pointer

  // Calling the speak function via the function pointer
  speaker.speak(); // Output: Speaking...

  return 0;
}
