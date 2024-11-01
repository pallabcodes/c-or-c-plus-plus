#include <iostream>

using namespace std;

// References as shortcuts
struct profile
{
    int id;
};

struct employee
{
    profile p;
};

// References can also be passed as a function parameter. I
void swap(int &p, int &q)
{
    int temp;
    temp = p;
    p = q;
    q = temp;
}

/**
 *  So far, C++ supports two types of variables:
 *
 * 1. An ordinary variable is a variable that contains the value of some type. e.g. we create a variable of type int i.e. int a = 2
 * 2. A pointer is a variable that stores the address of another variable. It can be dereferenced to retrieve the value to which this pointer points to.
 *
 * N.B: There is another variable that C++ supports, i.e., references. It is a variable that behaves as an alias for another variable.
 *
 * Needless to say, when we create a variable, then it occupies some memory location. and that Reference could be accessed using an ampersand (&) operator
 *
 * There are 2 types of References : a) Non-const values b) const values
 */

int main()
{
    // Referece on Non-const value
    int digit = 10;
    int &value = digit; // saving the reference i.e. memory address / location of into value by using &value

    cout << value << endl;

    // Reference as aliases
    int a = 10; // 'a' is a variable.
    int &b = a; // 'b' reference to a. initialized instatnly or later like this -> int &b; &b = a;
    int &c = a; // 'c' reference to a.

    int x1 = 70; // variable initialization
    int &y1 = x1;
    int &z1 = y1;
    cout << "Value of x1 is :" << x1 << endl;
    cout << "Value of y1 is :" << y1 << endl;
    cout << "Value of z1 is :" << z1 << endl;

    // Reassignement

    int x = 11;
    int z = 67;
    int &y = x; // y reference to x i.e. y holds the memory address / location of x
    int &y = z; // y reference to z, but throws a compile-time error because of the `reassignement`

    int x1 = 9;
    int x2 = 10;

    swap(x1, x2);

    cout << "value of a is :" << a << endl;
    cout << "value of b is :" << b << endl;

    // References as shortcuts

    employee e;
    int &ref = e.p.id;
    ref = 34;

    cout << e.p.id << endl;

    return 0;
}
