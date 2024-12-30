#include <iostream>

using namespace std;

int main()
{
    int *pNum = NULL;

    pNum = new int; // pNum i.e. pointer will be stored in the heap (instead of stack due to use of new)

    // Runtime = user generated input or system generated input

    // When don't know the input or actual size during compilation, since it depends on runtime input, then using heap makes sense

    *pNum = 21;

    cout << "The address: " << pNum << endl;

    cout << "The value: " << *pNum << '\n';

    delete pNum; // free up the memory when not needed

    char *pGrades = NULL;
    int size;

    cout << "How many grades to enter in ? ";
    cin >> size;

    pGrades = new char[size];

    for (int i = 0; i < size; i++)
    {
        cout << "index: " << i << endl;
        cout << "Enter grade #" << i + 1 << ": ";
        cin >> pGrades[i];
    }

    for (int i = 0; i < size; i++)
    {
        cout << pGrades[i] << " ";
    }

    delete[] pGrades;

    return 0;
}