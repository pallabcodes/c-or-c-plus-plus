// Smooth Sort: Adaptive heap sort variant
// Based on Dijkstra's research, O(n log n) worst case, O(n) best case for nearly sorted
// Space: O(1) extra space
// God modded implementation with Leonardo numbers optimization

#include <vector>
#include <iostream>
#include <algorithm>

// Leonardo numbers: L(0) = 1, L(1) = 1, L(n) = L(n-1) + L(n-2) + 1
int leonardo(int k) {
    if (k < 2) return 1;
    int a = 1, b = 1;
    for (int i = 2; i <= k; i++) {
        int temp = a + b + 1;
        a = b;
        b = temp;
    }
    return b;
}

class SmoothSort {
private:
    std::vector<int> heapSizes;
    
    void siftDown(std::vector<int>& arr, int root, int size) {
        int j = root;
        while (true) {
            int left = 2 * j + 1;
            int right = 2 * j + 2;
            
            if (left >= size) break;
            
            int maxChild = left;
            if (right < size && arr[right] > arr[left]) {
                maxChild = right;
            }
            
            if (arr[j] >= arr[maxChild]) break;
            
            std::swap(arr[j], arr[maxChild]);
            j = maxChild;
        }
    }
    
    void heapify(std::vector<int>& arr, int root, int size) {
        siftDown(arr, root, size);
    }
    
    void trinkle(std::vector<int>& arr, int p, int b, int c) {
        while (p > 0) {
            int r2 = p - 1;
            int t1 = r2;
            
            if (arr[t1] <= arr[r2]) {
                break;
            }
            
            if (c == 1) {
                std::swap(arr[p], arr[t1]);
                p = t1;
                continue;
            }
            
            int r1 = p - 1 - b;
            int t2 = r1;
            
            if (arr[t1] <= arr[r2] || arr[t2] <= arr[r2]) {
                if (arr[t1] > arr[t2]) {
                    std::swap(arr[p], arr[t1]);
                    p = t1;
                    b = b - c - 1;
                    c = c - 1;
                } else {
                    std::swap(arr[p], arr[t2]);
                    p = t2;
                    b = c;
                    c = 0;
                }
            } else {
                break;
            }
        }
        heapify(arr, p, b);
    }
    
    void semitrinkle(std::vector<int>& arr, int& p, int& b, int& c) {
        int r1 = p - 1 - b;
        if (arr[r1] > arr[p]) {
            std::swap(arr[p], arr[r1]);
            trinkle(arr, r1, b, c);
        }
    }
    
public:
    void sort(std::vector<int>& arr) {
        int n = arr.size();
        if (n <= 1) return;
        
        int q = 1, r = 0, p = 1, b = 1, c = 1;
        
        while (q < n) {
            if ((p & 7) == 3) {
                siftDown(arr, r, b);
                p = (p + 1) >> 2;
                b = b + c + 1;
                c = 1;
            } else if ((p & 3) == 1) {
                if (q + c < n) {
                    siftDown(arr, r, b);
                } else {
                    trinkle(arr, r, b, c);
                }
                
                do {
                    p = p << 1;
                    b = b - c - 1;
                } while (b > 1);
                p++;
            }
            
            q++;
            r++;
        }
        
        trinkle(arr, r, b, c);
        
        while (q > 1) {
            q--;
            if (b == 1) {
                r--;
                p--;
                while ((p & 1) == 0) {
                    p = p >> 1;
                    b = b + c + 1;
                    c = c - 1;
                }
            } else if (b >= 3) {
                p--;
                r = r - b + c;
                b = b - c - 1;
                
                if (p > 0) {
                    semitrinkle(arr, r, b, c);
                }
                
                p = (p << 1) + 1;
                r = r + c;
                b = b + c + 1;
                c = 0;
                
                semitrinkle(arr, r, b, c);
            }
        }
    }
};

void smoothSort(std::vector<int>& arr) {
    SmoothSort sorter;
    sorter.sort(arr);
}

// Example usage
int main() {
    std::vector<int> arr = {64, 34, 25, 12, 22, 11, 90, 5, 77, 1};
    
    std::cout << "Original array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << std::endl;
    
    smoothSort(arr);
    
    std::cout << "Sorted array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << std::endl;
    
    return 0;
}

