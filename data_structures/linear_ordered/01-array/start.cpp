#include <iostream>

using namespace std;

// Function to declare, initialize, and print a 1D integer array
void print1DArray()
{
  // 1D array declaration with a size of 5
  int arr[5];

  // Initialize the 1D array using a formula inside a for loop
  for (int i = 0; i < 5; i++)
  {

    // so, here it iterates from 0 to 4 and so e.g. for i = 0 it is (0×0)−(2×0)+1 = 0−0+1 = 1

    arr[i] = (i * i) - (2 * i) + 1; // Assign each element using the formula (i^2 - 2i + 1)
  }

  printf("Elements of Array: "); // Print a label for the output

  // Traverse the array to print each element
  for (int i = 0; i < 5; i++)
  {
    printf("%d ", arr[i]); // Print the current element followed by a space
  }
}

// Function to declare, initialize, and print a 2D array
void print2DArray()
{
  // Declaring and initializing a 2D array
  int arr[2][3] = {10, 20, 30, 40, 50, 60};

  // Calculate the number of rows and columns dynamically
  int rows = sizeof(arr) / sizeof(arr[0]);          // Total size divided by the size of one row
  int columns = sizeof(arr[0]) / sizeof(arr[0][0]); // Size of one row divided by the size of one element

  printf("2D Array:\n"); // Print a label indicating the start of the 2D array output

  // Loop through the rows and columns using the calculated sizes
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < columns; j++)
    {
      printf("%d ", arr[i][j]); // Print the current element with a space
    }
    printf("\n"); // Print a newline after each row for better readability
  }
}

// Function to declare, initialize, and print a 3D array
void print3DArray()
{
  // Declare and initialize a 3D array
  int arr[2][2][2] = {
      {{10, 20}, {30, 40}},
      {{50, 60}, {70, 80}}};

  // Calculate dimensions dynamically
  int depth = sizeof(arr) / sizeof(arr[0]);               // Number of 2D matrices (depth)
  int rows = sizeof(arr[0]) / sizeof(arr[0][0]);          // Number of rows per 2D matrix
  int columns = sizeof(arr[0][0]) / sizeof(arr[0][0][0]); // Number of columns per row

  // Print elements of the 3D array
  printf("3D Array Elements:\n");
  for (int i = 0; i < depth; i++)
  {
    printf("Matrix %d:\n", i + 1); // Label for each 2D matrix
    for (int j = 0; j < rows; j++)
    {
      for (int k = 0; k < columns; k++)
      {
        printf("%d ", arr[i][j][k]); // Print each element in the current matrix
      }
      printf("\n"); // Newline after each row
    }
    printf("\n"); // Additional newline after each matrix
  }
}

// Relationship between array and pointers
// Function to demonstrate the relationship between arrays and pointers
void demonstrateArrayPointerRelation()
{
  // Declare and initialize an integer array
  int arr[5] = {10, 20, 30, 40, 50};

  // Pointer to the first element of the array
  int *ptr = &arr[0];

  // Compare the address stored in the array name and the address of the first element
  // Memory address of arr and arr[0] is the same because, in C and C++, the name of the array arr is a pointer to its first element (&arr[0]).
  cout << "Address Stored in Array name: " << arr
       << "\nAddress of 1st Array Element: " << &arr[0] << endl;

  // Print array elements using pointer arithmetic
  cout << "Array elements using pointer: ";
  for (int i = 0; i < 5; i++)
  {
    cout << *ptr++ << " "; // Dereference the pointer to get the current element, then increment it
  }
  cout << endl; // Add a newline for better readability
}

// # Passing an Array to a Function

// In `main()`, `sizeof(arr)` gives the total size of the array (20 bytes for 5 int elements).
// In `printArray()`, `arr` is a pointer on the first element, so `sizeof(arr)` returns the size of the pointer (typically 8 bytes on a 64-bit system).
// N.B: once again, on an array its pointer (by default) on its first / 0th element so when passed the array the pointer is first element too

// Function to print an array and its size
void printArray(int arr[], size_t size)
{
  // Print the size of the array passed to the function
  // Used %zu: for correct format specifier for size_t ensures no warnings or type mismatches.
  printf("Size of Array in Functions: %zu\n", size); // Use size parameter here

  // Print array elements
  printf("Array Elements: ");
  for (size_t i = 0; i < size; i++)
  {
    printf("%d ", arr[i]);
  }
  printf("\n");
}

// # Return an Array from a Function
int *func()
{
  static int arr[5] = {1, 2, 3, 4, 5};

  return arr;
}

void noOutOfBound()
{
  int arr[2] = {10, 20}; // indexes are 0, 1

  // Avoid assigning values to out of bounds indices
  // arr[2] = 30;  // This would cause undefined behavior
  // arr[3] = 40;  // This would cause undefined behavior

  // Correct output for array indices 0 and 1
  printf("%d ", arr[0]);
  printf("%d ", arr[1]);
  printf("\n");
}

// print the average from the given array

float getAvg(float *arr, int size)
{
  int sum = 0;

  for (int i = 0; i < size; i++)
  {
    sum += arr[i];
  }

  return sum / size;
}

float getMax(float *arr, int size)
{
  int max = 0;

  for (int i = 0; i < size; i++)
  {
    if (arr[i] > max)
    {
      max = arr[i];
    };
  }

  return max;
}

// Function to read input into an array
void inputArray(int arr[], int size)
{
  printf("Enter %d elements: ", size);
  for (int i = 0; i < size; i++)
  {
    scanf("%d", &arr[i]); // Take input for each element
  }
}

// Function to print array elements
void printArrayElements(const int arr[], int size)
{
  printf("Array Elements: ");
  for (int i = 0; i < size; i++)
  {
    printf("%d ", arr[i]); // Print each element
  }
  printf("\n"); // Add a newline for better formatting
}

int main()
{
  // creating array of character
  char arr[6] = {'G', 'e', 'e', 'k', 's', '\0'};
  // Define a fixed-size character array with 6 elements.
  // Initialize it with the characters 'G', 'e', 'e', 'k', 's', and the null terminator '\0' to make it a valid C-style string.

  // printing string
  int i = 0;     // Initialize an integer variable 'i' to 0. This will be used as an index for the array.
  while (arr[i]) // Loop until the current character in 'arr' is not the null terminator '\0'.
  {
    printf("%c", arr[i++]); // Print the character at index 'i', then increment 'i' to move to the next character.
  }

  // Declare and initialize an array
  int digits[5] = {10, 20, 30, 40, 50};

  // Print the size of the array in the main function
  printf("Size of Array in main(): %zu\n", sizeof(digits));

  // Pass the array and its size to the function
  printArray(digits, sizeof(digits) / sizeof(digits[0]));

  print1DArray();
  print2DArray();
  print3DArray();
  demonstrateArrayPointerRelation();

  noOutOfBound();

  // Since, func() returns a pointer and i.e. saved to t
  int *ptr = func();

  printf("Array Elements: ");
  for (int i = 0; i < 5; i++)
  {
    printf("%d ", *ptr++);
  }

  // cout << "The maximum no is " << getMax(digits, sizeof(digits) / sizeof(digits[0]));

  int list[5];

  inputArray(list, 5);         // Function call to take input
  printArrayElements(list, 5); // Function call to print elements

  float list2[5] = {10, 20, 30, 40, 50};
  // size of array using sizeof operator
  int n = sizeof(list2) / sizeof(float); // mostly used & does same  sizeof(list2) / sizeof(list2[0]);

  // printing array elements
  printf("Array Elements: ");
  for (int i = 0; i < n; i++)
  {
    printf("%.0f ", list2[i]);
  }

  // calling getAverage function and printing average
  printf("\nAverage: %.2f", getAvg(list2, n));

  return 0; // Exit the program successfully.
}