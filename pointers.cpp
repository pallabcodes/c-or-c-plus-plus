#include <iostream>

using namespace std;

// Function that takes an array and its size, printing each element
void fun(int arr[], int size)
{
    for (int i = 0; i < size; ++i)
    {
        cout << arr[i] << " ";
    }
    cout << endl;
}

int main()
{
    int digit = 30;
    int *p = nullptr; // Initialize pointer to null for safety

    cout << "p: " << p << endl;
    cout << "p memory address before assign: " << &p << endl;

    // Assign the memory address of `digit` to pointer `p`
    p = &digit;

    cout << "Address of digit variable is: " << &digit << endl;
    cout << "Address stored in p: " << p << endl;
    cout << "Value at p (digit's value): " << *p << endl; // dereference to get the actual value from p (i.e. a a pointer variable)

    // Swapping values of two variables using pointers
    int a = 20;
    int b = 10;
    int *p1 = &a;
    int *p2 = &b;

    cout << "Before swap: *p1=" << *p1 << " *p2=" << *p2 << endl;

    *p1 = *p1 + *p2; // p1 now holds the sum of a and b
    *p2 = *p1 - *p2; // p2 now holds the original value of a
    *p1 = *p1 - *p2; // p1 now holds the original value of b

    cout << "After swap: *p1=" << *p1 << " *p2=" << *p2 << endl;

    // Demonstrating a function call with an array and its size
    int digits[] = {1, 2, 3, 4, 5};                // Initialize an example array
    int size = sizeof(digits) / sizeof(digits[0]); // Calculate array size

    fun(digits, size); // Call the function with both array and size

    // Void pointer example and array of pointers
    int *ptr1 = nullptr; // Initialize ptr1 to null
    cout << "ptr1: " << ptr1 << endl;

    int *arr[10] = {}; // Initialize an array of int pointers with nullptrs

    // Double pointer (pointer to pointer) example
    int temp = 42;
    arr[2] = &temp;       // Initialize the third element of arr
    int **ptr2 = arr + 2; // ptr2 points to the address of the third pointer in arr

    int value = **ptr2; // Get the value pointed to by arr[2]

    cout << "ptr2: " << ptr2 << ", actual value: " << value << endl;

    return 0;
}
