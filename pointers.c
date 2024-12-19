#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -------------------------------
// 1. Introduction to Pointers
// -------------------------------

void basicsOfPointers()
{
  int num = 42;
  int *ptr = &num; // Pointer to num

  printf("Value of num: %d\n", num);
  printf("Address of num: %p\n", (void *)&num);
  printf("Value of ptr (address of num): %p\n", (void *)ptr);
  printf("Value pointed by ptr: %d\n", *ptr); // Dereferencing the pointer
}

// -------------------------------
// 2. Null Pointers
// -------------------------------

void nullPointer()
{
  int *nullPtr = NULL;

  if (nullPtr == NULL)
  {
    printf("nullPtr is NULL\n");
  }
}

// -------------------------------
// 3. Pointer Arithmetic
// -------------------------------

void pointerArithmetic()
{
  int arr[5] = {10, 20, 30, 40, 50};
  int *ptr = arr; // Points to the first element of the array

  printf("Pointer arithmetic:\n");
  for (int i = 0; i < 5; i++)
  {
    printf("Element %d: %d\n", i, *(ptr + i)); // Access using pointer arithmetic
  }
}

// -------------------------------
// 4. Pointers and Arrays
// -------------------------------

void pointersAndArrays()
{
  int arr[3] = {1, 2, 3};
  int *ptr = arr;

  printf("Array elements using pointers:\n");
  for (int i = 0; i < 3; i++)
  {
    printf("arr[%d] = %d\n", i, *(ptr + i));
  }
}

// -------------------------------
// 5. Dynamic Memory Allocation
// -------------------------------

void dynamicMemoryAllocation()
{
  int *dynArray = (int *)malloc(5 * sizeof(int)); // Allocate memory for 5 integers
  if (dynArray == NULL)
  {
    printf("Memory allocation failed!\n");
    return;
  }

  for (int i = 0; i < 5; i++)
  {
    dynArray[i] = i + 1; // Assign values
  }

  printf("Dynamic Array elements:\n");
  for (int i = 0; i < 5; i++)
  {
    printf("%d ", dynArray[i]);
  }
  printf("\n");

  free(dynArray); // Free allocated memory
}

// -------------------------------
// 6. Pointers to Pointers (Double Pointers)
// -------------------------------

void pointersToPointers()
{
  int num = 42;
  int *ptr = &num;
  int **ptrToPtr = &ptr;

  printf("Value of num: %d\n", num);
  printf("Value of ptr (address of num): %p\n", (void *)ptr);
  printf("Value of ptrToPtr (address of ptr): %p\n", (void *)ptrToPtr);
  printf("Value pointed by ptrToPtr: %d\n", **ptrToPtr);
}

// -------------------------------
// 7. Function Pointers
// -------------------------------

void printNumber(int num)
{
  printf("Number: %d\n", num);
}

void functionPointers()
{
  void (*funcPtr)(int) = &printNumber; // Pointer to function
  funcPtr(10);                         // Call the function using the pointer
}

// -------------------------------
// 8. Passing Pointers to Functions
// -------------------------------

void increment(int *ptr)
{
  (*ptr)++;
}

void passingPointersToFunctions()
{
  int num = 5;
  printf("Before increment: %d\n", num);
  increment(&num);
  printf("After increment: %d\n", num);
}

// -------------------------------
// 9. Arrays of Pointers
// -------------------------------

void arraysOfPointers()
{
  const char *arr[] = {"Hello", "World", "Pointers"};
  for (int i = 0; i < 3; i++)
  {
    printf("%s\n", arr[i]);
  }
}

// -------------------------------
// 10. Pointer to a String
// -------------------------------

void pointerToString()
{
  const char *str = "Hello, Pointer!";
  printf("String: %s\n", str);
}

// -------------------------------
// 11. Void Pointers
// -------------------------------

void voidPointers()
{
  int num = 42;
  void *ptr = &num;

  // Need to cast before dereferencing
  printf("Value of num using void pointer: %d\n", *(int *)ptr);
}

// -------------------------------
// 12. Dangling Pointers
// -------------------------------

void danglingPointers()
{
  int *danglingPtr = (int *)malloc(sizeof(int));
  *danglingPtr = 42;

  printf("Value before freeing: %d\n", *danglingPtr);

  free(danglingPtr);
  danglingPtr = NULL; // Set to NULL to avoid undefined behavior
}

// -------------------------------
// 13. Common Mistakes and Tips
// -------------------------------

void commonMistakesAndTips()
{
  // Forgetting to initialize pointers
  int *uninitPtr; // Uninitialized pointer
  // printf("%d\n", *uninitPtr); // Undefined behavior!

  // Forgetting to free dynamically allocated memory
  int *leakPtr = (int *)malloc(sizeof(int));
  if (leakPtr != NULL)
  {
    *leakPtr = 10;
    printf("LeakPtr Value: %d\n", *leakPtr);
    free(leakPtr); // Free the memory to avoid memory leak
  }
}

// -------------------------------
// 14. Const Pointers
// -------------------------------

void constPointers()
{
  int num = 42;
  int num2 = 100;

  const int *ptr1 = &num;       // Pointer to a constant (value can't be modified)
  int *const ptr2 = &num;       // Constant pointer (address can't change)
  const int *const ptr3 = &num; // Constant pointer to constant data

  printf("ptr1: %d\n", *ptr1);
  printf("ptr2: %d\n", *ptr2);
  printf("ptr3: %d\n", *ptr3);

  // *ptr1 = 100; // Error: value is constant
  // ptr2 = &num2; // Error: address is constant
}

// -------------------------------
// Main Function
// -------------------------------

int main()
{
  printf("1. Basics of Pointers\n");
  basicsOfPointers();

  printf("\n2. Null Pointers\n");
  nullPointer();

  printf("\n3. Pointer Arithmetic\n");
  pointerArithmetic();

  printf("\n4. Pointers and Arrays\n");
  pointersAndArrays();

  printf("\n5. Dynamic Memory Allocation\n");
  dynamicMemoryAllocation();

  printf("\n6. Pointers to Pointers\n");
  pointersToPointers();

  printf("\n7. Function Pointers\n");
  functionPointers();

  printf("\n8. Passing Pointers to Functions\n");
  passingPointersToFunctions();

  printf("\n9. Arrays of Pointers\n");
  arraysOfPointers();

  printf("\n10. Pointer to a String\n");
  pointerToString();

  printf("\n11. Void Pointers\n");
  voidPointers();

  printf("\n12. Dangling Pointers\n");
  danglingPointers();

  printf("\n13. Common Mistakes and Tips\n");
  commonMistakesAndTips();

  printf("\n14. Const Pointers\n");
  constPointers();

  return 0;
}
