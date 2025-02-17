#include <iostream>
#include <vector>
using namespace std;

void insertionSort(vector<int> &arr) {
    int n = arr.size();
    
    for (int i = 1; i < n; i++) {
        int key = arr[i];  // Current element to insert
        int j = i - 1;
        
        // Shift elements of arr[0..i-1] that are greater than key
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];  // Shift right
            j--;
        }
        
        arr[j + 1] = key;  // Insert key at the correct position
    }
}

int main() {
    vector<int> arr = {5, 3, 8, 4, 2, 7, 1, 9};

    insertionSort(arr);

    cout << "Sorted Array: ";
    for (int num : arr) {
        cout << num << " ";
    }
    cout << endl;

    return 0;
}
