#include <iostream>

class MyClass {
public:
    int doSomething(int arr[], int size) { 
        return doSomething(arr, size, true); 
    }

    int doSomething(int arr[], int size, bool flag) { 
        // Implementation 
        return 0; 
    }
};

int main() {
    int myArray[] = {1, 2, 3};
    MyClass obj;
    int result = obj.doSomething(myArray, 3); 
    std::cout << "Result: " << result << std::endl;
    return 0;
}