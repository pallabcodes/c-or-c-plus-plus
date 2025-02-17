#include <iostream>
#include <vector>
using namespace std;

const int RUN = 32; // Tim Sort uses small runs for Insertion Sort

// Insertion Sort for small chunks
void insertionSort(vector<int> &arr, int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int key = arr[i], j = i - 1;
        while (j >= left && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// Merge two sorted halves
void merge(vector<int> &arr, int l, int m, int r) {
    int len1 = m - l + 1, len2 = r - m;
    vector<int> left(arr.begin() + l, arr.begin() + m + 1);
    vector<int> right(arr.begin() + m + 1, arr.begin() + r + 1);

    int i = 0, j = 0, k = l;
    while (i < len1 && j < len2)
        arr[k++] = (left[i] < right[j]) ? left[i++] : right[j++];

    while (i < len1) arr[k++] = left[i++];
    while (j < len2) arr[k++] = right[j++];
}

// Tim Sort
void timSort(vector<int> &arr) {
    int n = arr.size();

    // Sort small chunks using Insertion Sort
    for (int i = 0; i < n; i += RUN)
        insertionSort(arr, i, min(i + RUN - 1, n - 1));

    // Merge chunks using Merge Sort
    for (int size = RUN; size < n; size *= 2) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = min(left + 2 * size - 1, n - 1);
            if (mid < right) merge(arr, left, mid, right);
        }
    }
}

// Driver Code
int main() {
    vector<int> arr = {5, 21, 7, 23, 19};
    timSort(arr);

    cout << "Sorted Array: ";
    for (int num : arr) cout << num << " ";
    cout << endl;
    return 0;
}
