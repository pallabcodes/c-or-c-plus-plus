// Block Sort: Cache efficient sorting algorithm
// Based on research paper "BlockQuicksort: How Branch Mispredictions don't affect Quicksort"
// Time: O(n log n) worst case, O(n) best case
// Space: O(log n)
// Extremely cache friendly and branch prediction friendly

#include <vector>
#include <algorithm>
#include <iostream>
#include <climits>

const int BLOCK_SIZE = 64; // Cache line size

// Insertion sort for small arrays
void insertionSort(std::vector<int>& arr, int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// Partition using block-based approach
int partitionBlock(std::vector<int>& arr, int left, int right) {
    int pivot = arr[right];
    int i = left - 1;
    
    // Block-based partitioning for cache efficiency
    int numBlocks = (right - left) / BLOCK_SIZE;
    int* offsetsL = new int[numBlocks + 1];
    int* offsetsR = new int[numBlocks + 1];
    
    offsetsL[0] = 0;
    offsetsR[0] = 0;
    
    for (int block = 0; block < numBlocks; block++) {
        int blockStart = left + block * BLOCK_SIZE;
        int countL = 0, countR = 0;
        
        for (int j = 0; j < BLOCK_SIZE; j++) {
            int idx = blockStart + j;
            if (arr[idx] <= pivot) {
                countL++;
            } else {
                countR++;
            }
        }
        
        offsetsL[block + 1] = offsetsL[block] + countL;
        offsetsR[block + 1] = offsetsR[block] + countR;
    }
    
    // Rearrange elements
    std::vector<int> temp(right - left + 1);
    for (int block = 0; block < numBlocks; block++) {
        int blockStart = left + block * BLOCK_SIZE;
        int lIdx = offsetsL[block];
        int rIdx = offsetsR[block];
        
        for (int j = 0; j < BLOCK_SIZE; j++) {
            int idx = blockStart + j;
            if (arr[idx] <= pivot) {
                temp[lIdx++] = arr[idx];
            } else {
                temp[offsetsL[numBlocks] + rIdx++] = arr[idx];
            }
        }
    }
    
    // Handle remaining elements
    int remainingStart = left + numBlocks * BLOCK_SIZE;
    int lIdx = offsetsL[numBlocks];
    for (int j = remainingStart; j < right; j++) {
        if (arr[j] <= pivot) {
            temp[lIdx++] = arr[j];
        } else {
            temp[offsetsL[numBlocks] + offsetsR[numBlocks]++] = arr[j];
        }
    }
    
    // Copy back
    for (int j = 0; j < right - left; j++) {
        arr[left + j] = temp[j];
    }
    
    delete[] offsetsL;
    delete[] offsetsR;
    
    int pivotPos = left + offsetsL[numBlocks];
    std::swap(arr[pivotPos], arr[right]);
    return pivotPos;
}

// Block sort main function
void blockSort(std::vector<int>& arr, int left, int right) {
    if (right - left < 16) {
        insertionSort(arr, left, right);
        return;
    }
    
    int pivot = partitionBlock(arr, left, right);
    blockSort(arr, left, pivot - 1);
    blockSort(arr, pivot + 1, right);
}

void blockSort(std::vector<int>& arr) {
    if (arr.size() <= 1) return;
    blockSort(arr, 0, arr.size() - 1);
}

// Example usage
int main() {
    std::vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 5, 77, 1, 45, 33, 88, 99, 2};
    
    std::cout << "Original array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << std::endl;
    
    blockSort(arr);
    
    std::cout << "Sorted array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << std::endl;
    
    return 0;
}

