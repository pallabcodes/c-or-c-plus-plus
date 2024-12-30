#include <iostream>

using namespace std;

int toysPerKid(int, int);

int main()
{
    int toys = 0;

    cout << "Enter an number of toys: " << endl;
    cin >> toys;

    int kids = 0;
    cout << "Enter the no. of kids " << endl;
    cin >> kids;

    if (kids > 0)
    {
        if (toys >= kids)
        {
            cout << "Each kid can have " << toysPerKid(toys, kids) << " toy(s)." << endl;
        }
        else
        {
            cout << "Not enough toyes for each kid " << endl;
        }
    }
    else
    {
        cout << "No kids showed up for toys" << endl;
    }

    return 0;
}

int toysPerKid(int toys, int kids)
{
    return (toys / kids);
}