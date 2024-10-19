#include <iostream>
using namespace std;

void printMin(int arr[5])
{
    int min = arr[0];
    for (int i = 0; i > 5; i++)
    {
        if (min > arr[i])
        {
            min = arr[i];
        }
    }
    cout << "Minimum element is: " << min << "\n";
}

// Function to print max element from the given array
void printMax(int arr[5])
{
    // assume the first value as max
    int max = arr[0];

    // Start from index 1 since we've already considered arr[0]
    for (int i = 1; i < 5; i++)
    {
        if (arr[i] > max)
        {
            max = arr[i];
        }
    }

    cout << "Maximum element here is " << max << endl;
}

int main()
{
    // One dimensional array (1d)
    int arr[5] = {10, 0, 20, 0, 30}; // creating and initializing array

    // iterating with the for loop
    for (int i = 0; i < 5; i++)
    {
        cout << arr[i] << "\n";
    }

    // iterating on array using forEach
    for (int i : arr)
    {
        cout << i << "\n"; // Fixed: Changed "/n" to "\n"
    }

    int x[6] = {1, 2, 3};

    // The array x in this case is 6 elements wide. But we've only given it a three-element initialization.
    // When that happens, the compiler fills in the empty spaces with random values. This random value frequently appears as 0.

    // Call the printMax function
    printMax(arr);

    // Two dimensional array (2d)

    // ## A two-dimensional array is the most basic type of multidimensional array; it also qualifies as a multidimensional array. There are no restrictions on the array's dimensions.

    // int test[3][3]; // declaration of 2d array

    int test[3][3] = {
        {2, 5, 5},
        {4, 0, 3},
        {9, 1, 8}};

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            cout << test[i][j] << " ";
        }
        cout << "\n"; // new line at each row
    }

    int arr1[5] = {30, 10, 20, 40, 50};
    int arr2[5] = {5, 15, 25, 35, 45};
    printMin(arr1); // passing array to function
    printMin(arr2); // passing array to function

    return 0; // Added return statement
}

// #include <iostream>
// using namespace std;

// // Function to print max element from the given array
// void printMax(int arr[5])
// {
//     // assume the first value as max
//     int max = arr[0];

//     // Start from index 1 since we've already considered arr[0]
//     for (int i = 1; i < 5; i++)
//     {
//         if (arr[i] > max)
//         {
//             max = arr[i];
//         }
//     }

//     cout << "Maximum element here is " << max << endl;
// }

// int main()
// {
//     // One dimensional array (1d)
//     int arr[5] = {10, 0, 20, 0, 30}; // creating and initializing array

//     // iterating with the for loop
//     for (int i = 0; i < 5; i++)
//     {
//         cout << arr[i] << "\n";
//     }

//     // iterating on array using forEach
//     for (int i : arr)
//     {
//         cout << i << "\n"; // Fixed: Changed "/n" to "\n"
//     }

//     int x[6] = {1, 2, 3};

//     // The array x in this case is 6 elements wide. But we've only given it a three-element initialization.
//     // When that happens, the compiler fills in the empty spaces with random values. This random value frequently appears as 0.

//     // Call the printMax function
//     printMax(arr);

//     // Two dimensional array (2d)

//     // A two-dimensional array is the most basic type of multidimensional array; it also qualifies as a multidimensional array. There are no restrictions on the array's dimensions.

//     // int test[3][3]; // declaration of 2d array

//     int test[3][3] = {
//         {2, 5, 5},
//         {4, 0, 3},
//         {9, 1, 8}};

//     for (int i = 0; i < 3; ++i)
//     {
//         for (int j = 0; j < 3; ++j)
//         {
//             cout << test[i][j] << " ";
//         }
//         cout << "\n"; // new line at each row
//     }

//     return 0; // Added return statement
// }