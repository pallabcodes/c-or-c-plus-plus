#include <iostream>
#include <vector>
using namespace std;

void shellSort(vector<int> &arr) {
    int n = arr.size();
    
    // Use decreasing gap sequence (Knuth’s sequence could be used too)
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            int temp = arr[i], j;
            
            // Insertion sort logic for elements at gap distance
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) {
                arr[j] = arr[j - gap];
            }
            arr[j] = temp;
        }
    }
}

// Driver Code
int main() {
    vector<int> arr = {12, 34, 54, 2, 3};
    shellSort(arr);

    cout << "Sorted Array: ";
    for (int num : arr) cout << num << " ";
    cout << endl;
    return 0;
}
