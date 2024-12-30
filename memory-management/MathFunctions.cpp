#include <iostream>
#include <string>
#include <cmath>
using namespace std;

const double PI = 3.14159;

// Function declarations
int getPosInt(string msg); 
double areaOfCircle(int r);
double volOfSphere(int r);


int getPosInt(string msg) {
    int num = 0;
    do {
        cout << msg << endl;
        cin >> num;
    } while (num <= 0);
    return num;
}

double areaOfCircle(int r) {
    return PI * pow(r, 2);
}

double volOfSphere(int r) {
    return (4.0 / 3.0) * PI * pow(r, 3); 
}