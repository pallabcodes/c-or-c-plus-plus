#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h> // Add this include for malloc and free

void dataTypes()
{
  // Examples for basic types
  int integerVar = 42;
  float floatVar = 3.14;
  double doubleVar = 2.718281828459045;
  char charVar = 'A';
  bool boolVar = true;

  // Dynamic memory allocation
  int *dynArray = (int *)malloc(5 * sizeof(int)); // Allocate memory for 5 integers

  if (dynArray == NULL)
  {
    printf("Memory allocation failed!\n");
    return;
  }

  for (int i = 0; i < 5; i++)
  {
    dynArray[i] = i + 1;
    printf("dynArray[%d] = %d\n", i, dynArray[i]);
  }

  free(dynArray); // Free allocated memory
}

int main()
{
  dataTypes();
  return 0;
}
