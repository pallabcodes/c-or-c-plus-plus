#include <iostream>
#include <vector>

using namespace std;

// Merge two sorted subarrays
void merge(vector<int> &arr, int left, int mid, int right) {
    int n1 = mid - left + 1, n2 = right - mid;
    vector<int> L(n1), R(n2);

    // Copy data to temp arrays L[] and R[]
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int i = 0; i < n2; i++) R[i] = arr[mid + 1 + i];

    // Merge the temp arrays back into arr[left..right]
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    }

    // Copy remaining elements of L[], if any
    while (i < n1) arr[k++] = L[i++];
    // Copy remaining elements of R[], if any
    while (j < n2) arr[k++] = R[j++];
}

// Merge Sort function
void mergeSort(vector<int> &arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2; // To prevent overflow

        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// Driver function
int main() {
    vector<int> arr = {38, 27, 43, 3, 9, 82, 10};

    mergeSort(arr, 0, arr.size() - 1);

    cout << "Sorted Array: ";
    for (int num : arr) cout << num << " ";
    cout << endl;

    return 0;
}
