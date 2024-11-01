#include <iostream>

using namespace std;

int doublePriority()
{
    int num1;
    double num2;
    // This is known as type promotion. since, double has a higher rank or priority
    cout << sizeof(num1 + num2);

    return 0;
}

int size()
{
    // Create a new integer and assign it the value 10
    int *ptr1 = new int(10);
    // Size of the pointer itself (ptr1), typically 8 bytes on a 64-bit system
    std::cout << "size of ptr1 : " << sizeof(ptr1) << std::endl;
    // Size of the integer value pointed to by ptr1, which is 4 bytes
    std::cout << "size of *ptr1 : " << sizeof(*ptr1) << std::endl;

    // Create a new char and assign it the value 'a'
    char *ptr2 = new char('a');
    // Size of the pointer itself (ptr2), typically 8 bytes on a 64-bit system
    std::cout << "size of ptr2 : " << sizeof(ptr2) << std::endl;
    // Size of the char value pointed to by ptr2, which is 1 byte
    std::cout << "size of *ptr2 : " << sizeof(*ptr2) << std::endl;

    // Create a new double and assign it the value 12.78
    double *ptr3 = new double(12.78);
    // Size of the pointer itself (ptr3), typically 8 bytes on a 64-bit system
    std::cout << "size of ptr3 : " << sizeof(ptr3) << std::endl;
    // Size of the double value pointed to by ptr3, which is 8 bytes
    std::cout << "size of *ptr3 : " << sizeof(*ptr3) << std::endl;

    // Clean up dynamically allocated memory
    delete ptr1;
    delete ptr2;
    delete ptr3;

    return 0;
}

void fun(int arr[], int size)
{
    cout << "Size of array is : " << size << endl;
}

class Base
{
    int a;
    int d;
    char ch;
};

int main()
{
    int digit = 30;

    // a pointer variable can point to a memory address which holds int data
    int *p = &digit; // Initialize p with the address of digit

    cout << "p " << p << endl;
    cout << "p memory address before assign " << &p << endl; // 0xe1c6dffd90

    // Here, p has been assigned with the memory address of digit e.g. 0xe1c6dffd9c
    p = &digit; // a pointer stores the memory address (so stored the memory address of digit)

    cout << "Address of digit variable is " << &digit << endl; // 0xe1c6dffd9c

    cout << "Address of p variable is " << p << endl; // 0xe1c6dffd9c

    // when a pointer variable that holds an address, by using asterisk, get the originally stored from that assigned memory address so with this actual value of digit that assigned to p is now accessible which is why below should print 30
    cout << "Value of p variable is:" << *p << endl;

    // pointer program to swap values 2 variables and this is how to do it

    int a = 20;
    int b = 10;
    int *p1 = &a;
    int *p2 = &b;

    cout << "Before swap: *p1=" << *p1 << " *p2=" << *p2 << endl;
    *p1 = *p1 + *p2;                                            // as the value here is 30 but since it is assigned  a pointer variable so its memory address is assigned instead of value
    cout << "p1 value: " << p1 << endl;                         // which is why here it will show the memory address not the value
    cout << "p1 addition with memory address: " << *p1 << endl; // but since here used asterisk so then can get the actual value i.e. 30
    *p2 = *p1 - *p2;                                            // once again *p1 means 30 and *p2 means 10 so 30 - 10 = 20, but since saved to pointer thus its memory address will be assigned
    cout << "p2 value: " << p2 << endl;                         // so, here it should print memory address
    cout << "p2 addition with memory address: " << *p2 << endl; // but since here used asterisk so here it should actual value i.e. 20

    // once again, 2 steps process happens here

    // 1. *p1 = 30, *p2 = 20

    // 2. *p1 = 30 - 20; the value is assigned to a pointer variable so its memory address will be assigned to *p1

    *p1 = *p1 - *p2;
    cout << "p1 subtraction with memory address: " << *p1 << endl; // now, using asterisk to get the actual value assigned i.e. 10

    cout << "After swap: ∗p1=" << *p1 << " *p2=" << *p2 << endl;

    // void pointers: the pointer that points to a value that has no type

    // int *ptr1; // ptr1 is not initalized therefore making it invalid
    // cout << ptr1 << endl;
    // int *arr[10]; // arr is a fixed size of length 10, not initalized
    // int *ptr2 = arr + 20; // invalid since there is no element to access on 20th index when arr.length = 10 so, possible only (0 - 9) as below
    // int *ptr2 = arr + 2; // ptr2 now points to the third element of arr (index 2)

    // The above code has issue with void pointers, so below is the correct version

    int *ptr1 = nullptr;               // Initialize ptr1 to null
    cout << "here : " << ptr1 << endl; // ptr1 = 0, that's how it works when nullptr assigned to int pointer variable like *ptr1

    int *arr[10] = {}; // Initialize all elements to nullptr and here array will a length of 10, where each value's type int pointers

    // why the double asterisk or "pointer to pointer" ?  So ptr2 becomes a pointer to a pointer to an int.
    int **ptr2 = arr; // ptr2 points to the address of the third pointer in arr

    // If you want ptr2 to be int*, and assuming arr elements are properly initialized:
    // int *ptr2 = arr[2]; // This would be correct if arr[2] was initialized

    // int value = **ptr2; // This would get the int value, if arr[2] was initialized

    // cout << "ptr2: " << ptr2 << "actual value: " << value << endl;

    if (arr[2])
    { // Ensure arr[2] is initialized before dereferencing
        int value = **ptr2;
        cout << "ptr2: " << ptr2 << " actual value: " << value << endl;
    }
    else
    {
        cout << "arr[2] is not initialized, skipping dereference" << endl;
    }

    // sizeof() -> Determining the space in bytes occupied by each data type.
    cout << "Size of integer data type : " << sizeof(int) << endl;
    cout << "Size of float data type : " << sizeof(float) << endl;
    cout << "Size of double data type : " << sizeof(double) << endl;
    cout << "Size of char data type : " << sizeof(char) << endl;

    Base base;

    cout << "Size of class data type : " << sizeof(base) << endl;

    int digits[] = {10, 20, 30, 40, 50};

    // 20 since each int has 4 bytes
    cout << "Size of the array 'digits' is : " << sizeof(digits) << endl;

    // so, when an array is given as argument like here, it will internally & implicitly turned into a pointer, so on `64 bit system it's 8 bytes and 32-bits it's 4 bytes`
    fun(digits, sizeof(digits) / sizeof(digits[0]));

    size();

    doublePriority();

    return 0;
}
