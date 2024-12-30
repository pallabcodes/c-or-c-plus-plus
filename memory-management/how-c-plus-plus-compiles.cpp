#include <iostream>
#include <string>
#include <cmath>
#include "MyMathFunctions.h" // Include the header file before using namespace std

using namespace std;

int main()
{
    int radius = getPosInt("Enter a positive integer for the radius of a circle/sphere");

    double aCircle = areaOfCircle(radius);
    double vSphere  = volOfSphere(radius);

    return 0;
}