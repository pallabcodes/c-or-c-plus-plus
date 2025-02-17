#include <iostream>
#include <vector>
#include <algorithm> // For max_element

using namespace std;

// Counting Sort based on a specific digit (place)
void countingSort(vector<int> &arr, int place) {
    int n = arr.size();
    vector<int> output(n);  // Output array
    int count[10] = {0};    // Count array for digits 0-9

    // 1. Count occurrences of each digit at 'place'
    for (int num : arr) {
        int digit = (num / place) % 10;
        count[digit]++;
    }

    // 2. Compute cumulative count (for stable sorting)
    for (int i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }

    // 3. Place elements into output array in sorted order
    for (int i = n - 1; i >= 0; i--) {
        int digit = (arr[i] / place) % 10;
        output[count[digit] - 1] = arr[i];
        count[digit]--;
    }

    // 4. Copy sorted elements back to original array
    arr = output;
}

// Main Radix Sort function
void radixSort(vector<int> &arr) {
    if (arr.empty()) return;  // Edge case: empty array

    // Find the maximum number to determine the number of digits
    int maxVal = *max_element(arr.begin(), arr.end());

    // Sort based on each digit's place value (1s, 10s, 100s, ...)
    for (int place = 1; maxVal / place > 0; place *= 10) {
        countingSort(arr, place);
    }
}

// Driver function
int main() {
    vector<int> arr = {170, 45, 75, 90, 802, 24, 2, 66};

    radixSort(arr);

    cout << "Sorted Array: ";
    for (int num : arr) {
        cout << num << " ";
    }
    cout << endl;

    return 0;
}
