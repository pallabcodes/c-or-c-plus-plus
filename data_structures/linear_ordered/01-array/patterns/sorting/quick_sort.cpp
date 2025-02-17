#include <iostream>
#include <vector>

using namespace std;

// Partition function (Lomuto's partitioning)
int partition(vector<int> &arr, int low, int high) {
    int pivot = arr[high]; // Pivot as the last element
    int i = low - 1;       // Pointer for smaller elements

    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            swap(arr[++i], arr[j]); // Swap smaller element to left side
        }
    }
    swap(arr[i + 1], arr[high]); // Place pivot in correct position
    return i + 1;
}

// QuickSort function
void quickSort(vector<int> &arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);

        quickSort(arr, low, pi - 1);  // Sort left of pivot
        quickSort(arr, pi + 1, high); // Sort right of pivot
    }
}

// Driver function
int main() {
    vector<int> arr = {10, 7, 8, 9, 1, 5};

    quickSort(arr, 0, arr.size() - 1);

    cout << "Sorted Array: ";
    for (int num : arr) cout << num << " ";
    cout << endl;

    return 0;
}
