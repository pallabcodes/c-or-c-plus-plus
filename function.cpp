#include <iostream>

using namespace std;

// from the js perspective, think of funct as below:

/**
 * JavaScript equivalent would be something like this for the below func
function createFunc() {
    let i = 0;  // This creates closure-like behavior
    return function() {
        let j = 0;
        i++;
        j++;
        console.log(`i=${i} and j=${j}`);
    }
}
const func = createFunc();
 *
 *
*/

void func()
{
    /* N.B: Static variables in C++ maintain their value between function calls by storing them in persistent memory, so `i` remembers its previous value and increments from there - similar to closure behavior but achieved through static storage rather than lexical scoping. */
    static int i = 0;
    int j = 0;

    i++;
    j++;

    cout << "i=" << i << " and j=" << j << endl;
}

// There are two ways pass value or data to a function a) call by value b) call by reference

// call by value
void change(int data);

// call by reference
void swap(int *x, int *y)
{
    // since x, y passed as memory address to hold that, it must be pointers which is why * and cout will show the memory address
    cout << x << " " << y << endl;
    // as known, when a variable that hold memory address with pointers (as parameters), used pointers again then get its og value
    cout << *x << " " << *y << endl;
    int swap; // here, default value is 0
    swap = *x;
    *x = *y;
    *y = swap;
}

void change(int data)
{
    data = 5;
    cout << "Value of the data is (within function): " << data << endl;
}

// recursion : a) direct recursion b) indirect recursion

// direct recursion : when a function calls itself
void directRecursiveFunction(int n)
{
    if (n > 0)
    {
        cout << n << " ";
        directRecursiveFunction(n - 1); // Direct recursion
    }
}

// Forward declaration to avoid compilation error
void departmentA(int task);

// Function for Department B (handles even tasks)
void departmentB(int task)
{
    if (task <= 0)
    {
        // Base case: when task is 0, stop the recursion
        return;
    }
    cout << "Department B handling task " << task << endl;
    // Call Department A with the next odd-numbered task
    departmentA(task - 1);
}

// Function for Department A (handles odd tasks)
void departmentA(int task)
{
    if (task <= 0)
    {
        // Base case: when task is 0, stop the recursion
        return;
    }
    cout << "Department A handling task " << task << endl;
    // Call Department B with the next even-numbered task
    departmentB(task - 1);
}

// Declaring an extern variable 'x'
extern int x;

void externStorageClass()
{
    cout << "Understanding the extern class\n";

    // Accessing the extern variable 'x'
    cout << "Value of the variable 'x', declared as extern: " << x << "\n";

    // Modifying the value of extern variable 'x'
    x = 2;

    // Displaying the modified value of extern variable 'x'
    cout << "Modified value of the variable 'x', declared as extern: " << x;
}

// Defining the extern variable 'x'
int x = 0;

void autoStorageClass()
{
    cout << "Understanding the auto storage class\n";

    // Declaring variables with auto storage class
    int a = 32;
    float b = 3.2;
    // Below indicates that the string literal should not be modified. With this change, the code will correctly reflect to a constant string literal.
    const char *c = "JavaScript";
    char d = 'G';

    // Displaying the values of auto variables
    cout << a << " \n";
    cout << b << " \n";
    cout << c << " \n";
    cout << d << " \n";
}

// Function containing static variable
int staticFun()
{
    cout << "For static variables: ";
    static int count = 0;
    count++;
    return count;
}

// Function containing non-static variable
int nonStaticFun()
{
    cout << "For Non-Static variables: ";

    int count = 0;
    count++;
    return count;
}

void registerStorageClass()
{
    cout << "Illustrating the register class\n";

    // Declaring a register variable (no need to use register keyword)
    char b = 'G';

    // Displaying the value of the register variable 'b'
    cout << "Value of the variable 'b' declared as register: " << b << endl;
}

class Test
{
public:
    int x;
    mutable int y;

    Test()
    {
        x = 4;
        y = 10;
    }
};

int main()
{
    func();
    func();
    func();

    // call by value example
    int data = 3;
    change(data);
    cout << "Value of the data is (original value or data unaffected): " << data << endl;

    // call by reference example
    int x = 500, y = 100;
    swap(&x, &y); // passing value to function
    cout << "Value of x is: " << x << endl;
    cout << "Value of y is: " << y << endl;

    int task = 5;      // Start with an odd task (example input)
    departmentA(task); // Start recursion from Department A

    // example of the auto-storage class
    autoStorageClass();

    // Example of extern Storage Class
    externStorageClass();

    // Calling functions with static variables
    cout << staticFun() << "\n";
    cout << staticFun() << "\n";

    // Calling functions with non-static variables
    cout << nonStaticFun() << "\n";
    cout << nonStaticFun() << "\n";

    // Demonstrating the register Storage Class
    registerStorageClass();

    const Test t1;
    t1.y = 20;    // Mutable member can be modified in a const object
    cout << t1.y; // Output: 20

    return 0;
}

// review needed: https://www.javatpoint.com/cpp-storage-classes (auto, static, register, mutable, extern)